#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "webCam.h"
#include "StillCamera.h"
#include "SoundRecord.h"
#include "Nrf24.h"
#include "define.h"
#include "ftplib.h"
#include "Linker.h"
#include "Recording.h"
#include "Communication.h"

#define MAX_SIZE_UDP 1024
#define FTP_TEST_USER "test"
#define FTP_TEST_PASSWORD "pass"


//// MSB FIRST
using namespace std;

typedef unsigned char BYTE ;


/******** -------------------------- 	Functions -------------------------- ********/
bool ftpSendFile();
bool stopRecording();
bool startRecording( uint64_t idRecording );
void* ftpSenderThread( void* data );
void REC_TO_SRV_ParringStatus( bool succes );
void REC_TO_SRV_IdRecording();
void REC_TO_SRV_RECORDING_END(uint64_t idRecording);
void REC_TO_SRV_AckImage();
void REC_TO_SRV_Image( BYTE idCam );
bool getImageFromCam ( BYTE idCam );
void REC_TO_SRV_TcpAck( BYTE command );
void REC_TO_SRV_ImagePart( int noPart );
void REC_TO_SRV_Authenification( uint64_t idTag );

// Nrf
void ACT_TO_REC_ACK(BYTE data[], int size);
void ACT_TO_REC_StatusAns( BYTE data[], int size);
void ACT_TO_REC_StatusRequest( BYTE data[], int size);
void ACT_TO_REC_ControlRequest( BYTE data[], int size);
void ACT_TO_REC_PoolingParkingAns(BYTE data[], int size);
void ACT_TO_REC_Authentification( BYTE data[], int size );
void ACT_TO_REC_ValidateParringConfig(BYTE data[], int size);

// Udp
void SRV_TO_REC_AckImage ( BYTE data[], int size );
void SRV_TO_REC_InfoSrvAns ( BYTE data[], int size );

// Tcp
void SRV_TO_REC_VisuCam( BYTE* data, unsigned long size);
void SRV_TO_REC_StorageAns( BYTE* data, unsigned long size);
void SRV_TO_REC_ParringAsk( BYTE* data, unsigned long size);
void SRV_TO_REC_StatutRequest( BYTE* data, unsigned long size);
void SRV_TO_REC_GetIdRecording( BYTE* data, unsigned long size);
void SRV_TO_REC_FileTransfertAck( BYTE* data, unsigned long size );
void SRV_TO_REC_Authentification( BYTE* data, unsigned long size );



/******** -------------------------- 	Global Variables -------------------------- ********/
/// Status
bool _WaitAckImage;
bool isParking = false;
bool isRecording = false;
BYTE _StateArduino = 0x00;
BYTE _StateRaspberry = 0x00;

/// Temp
// Network
InfoTCP *_TcpInfo;
BYTE idGenerate[2];
uint64_t addressGenerate[] = { 0x012345,0x123456  };


// Image
char* _ImageContent = NULL;
uint16_t _ImageParts = 0, _ImageSize = 0;
// Recording
Recording *_CurrentRecording = NULL;
uint64_t _IdUserRecorder = 0x00;
uint64_t _IdRecording = 0x00;

/// FTP
string _FtpUser = "test";
string _FtpPassword = "pass";
string _FtpSendFileName = "";
string _FtpHost = "192.168.43.90:1919";
bool _FtpWaitStorageAns = false;
bool _FtpWaitAckMd5 = false;
bool _FtpGetImageOk = false;
bool _FtpCanUploadFile = false;
bool _isSending = false;
ftplib *_Ftp = new ftplib();
uint16_t _FtpPort = 1904;
pthread_t _FtpThread;
RecordingFile *fileInUpload = NULL;


/******** -------------------------- 	Initialisation -------------------------- ********/
/** initRecording
 * Loads Recordings from file
 **/
bool initRecording()
{
	return Recording::loadRecordings();
}

/** initNrfCallBacks
 * Configure Nrf and links to CallBack's functions
 **/
void initNrfCallBacks()
{
	if ( Nrf24::_Nrf == NULL )
	{
		LOGGER_ERROR("Nrf is Null");
		return;
	}
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_ACK,ACK);
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_Authentification, AUTH_ASK);
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_StatusRequest, STATUS_ASK);
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_StatusAns, STATUS_ANS);
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_ControlRequest, ACTIVATE_RECORDING);
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_PoolingParkingAns, POOL_PARK_ADDR_ANS);
	Nrf24::_Nrf->setFunction(&ACT_TO_REC_ValidateParringConfig, POOL_VALIDATE_CONFIG);
	Nrf24::_Nrf->setTransactionErrorFunction(&transactionError);
}

/** initUdpCallBacks
 * 	Configure Udp and links to CallBack's functions
 **/
void initUdpCallBacks()
{
	if ( Udp::_udp == NULL )
	{
		LOGGER_ERROR("Udp is Null");
		return;
	}
	_TcpInfo = Udp::_TcpInfo;
	Udp::_udp->setFunction( (FuncType) &SRV_TO_REC_InfoSrvAns, (int)GET_INFO_SRV );
	Udp::_udp->setFunction( (FuncType) &SRV_TO_REC_AckImage, (int) GET_IMAGE_ACK );
}

/** initTcpCallBacks
 * 	Configure Tcp and links to CallBack's functions
 **/
void initTcpCallBacks()
{
	if ( Tcp::_tcp == NULL )
	{
		LOGGER_ERROR("Tcp is Null");
		return;
	}
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_VisuCam, (int)ASK_VISU_CAM );
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_ParringAsk, (int)ASK_PARIRING );
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_Authentification, (int)GET_AUTHENTIFICATION_ANS_TCP );
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_StatutRequest, (int)ASK_STATUS_TCP );
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_StorageAns, (int)GET_STORAGE_ANS );
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_FileTransfertAck, (int)ACK_FILE_SEND_TCP );
	Tcp::_tcp->setFunction( (FuncType) &SRV_TO_REC_GetIdRecording, (int)ANS_ID_RECORDING );

	// Set FTP User
	char s[13];
	getMac( (char * )CurrentApplicationConfig.UDP_interface.c_str() , s );
	s[12] = 0;
	_FtpUser = SSTR( s );
}





/******** -------------------------- 	Common -------------------------- ********/
void transactionError( BYTE data[], int size )
{
	char tmp[250];
	tmp[0] = '\0';
	for(int i = 0; i < size; i++)
		sprintf(tmp,"%s %02X",tmp,data[i]);

	LOGGER_ERROR("Transaction Error ! Receive ["<< size <<"] :" << tmp );
}



void forceStartRecording()
{
	startRecording(1);
}
void forceStopRecording()
{
	stopRecording();
}




/******** -------------------------- 	Request -------------------------- ********/
/** ParringNrf
 *	Set Module in NRF Parking Mode. Send pooling Frame
 **/
void ParringNrf()
{
	LOGGER_INFO("Starting Parring");
	Nrf24::_Nrf->clearCommunications();
	isParking = true;

	// TODO : UPDATE MODULE STATUS
	Nrf24::_Nrf->updateAddr(ADDR_PARKING_RX,ADDR_PARKING_TX, false);
	BYTE answer[] = { POOL_PARK_ADDR } ;
	Nrf24::_Nrf->sendBuffer(POOL_PARK_ADDR,answer,1,false, 10);
}

/** REC_TO_SRV_AckImage
 * Send to Server a Frame to signal end of Image Transfert
 **/
void REC_TO_SRV_AckImage()
{
	BYTE datas[ ] = { SEND_IMAGE_COMPLETLY_SEND , (uint8_t) ((_ImageParts >> 8)&0xFF), (uint8_t)( _ImageParts & 0xFF) };
	_WaitAckImage = true;

	// TODO : INCLUDE IN A THREAD WITH MAX RETRY
	//while ( _WaitAckImage )
	{
		usleep(350000);
		Udp::_udp->send(datas,3,_TcpInfo->ipAddress);
	}
}

/** REC_TO_SRV_allFilesSends
 * Send a Frame to Server for signal that all file have been uploaded for the recording
 **/
void REC_TO_SRV_allFilesSends( uint64_t idRecording )
{
	BYTE res[] = { ALL_FILES_SEND,
				   (uint8_t) (( idRecording>>56) & 0xFF),
				   (uint8_t) (( idRecording>>48) & 0xFF),
				   (uint8_t) (( idRecording>>40) & 0xFF),
				   (uint8_t) (( idRecording>>32) & 0xFF),
				   (uint8_t) (( idRecording>>24) & 0xFF),
				   (uint8_t) (( idRecording>>16) & 0xFF),
				   (uint8_t) (( idRecording>>8 ) & 0xFF),
				   (uint8_t) (  idRecording      & 0xFF)
				 };

	Tcp::_tcp->sendData(res,9,true);
}

/** REC_TO_SRV_ImagePart
 *	Send to Server a part of image
 **/
void REC_TO_SRV_ImagePart( int noPart )
{
	BYTE datas[ MAX_SIZE_UDP+3 ];
	datas[0] = SEND_IMAGE;

	datas[1] = (noPart >> 8)&0xFF;
	datas[2] = noPart & 0xFF;

	if ( noPart != ( _ImageParts-1) )
	{
		memcpy(datas+3,_ImageContent+(noPart*MAX_SIZE_UDP),MAX_SIZE_UDP);
		Udp::_udp->send(datas,MAX_SIZE_UDP+3,_TcpInfo->ipAddress);
	}
	else
	{
		memset(datas+3,0,MAX_SIZE_UDP);
		int s = _ImageSize - (noPart*MAX_SIZE_UDP);
		memcpy(datas+3,_ImageContent+(noPart*MAX_SIZE_UDP),s);
		Udp::_udp->send(datas,s+3,_TcpInfo->ipAddress);
	}
	usleep(15);
}

/** REC_TO_SRV_Image
 * Send Image of a camera to Server
 **/
void REC_TO_SRV_Image( BYTE idCam )
{
	// Ignore if Tcp not connected
	if ( _TcpInfo->port == -1 )
	{
		return;
	}

	if ( isRecording )
	{
		LOGGER_WARN("Can't get image: Currently recording");
		return;
	}
	// Load image from Camera
	if ( getImageFromCam( idCam ) == false )
	{
		LOGGER_WARN("Can't get image");
		return;
	}

	_ImageParts = (_ImageSize)/(MAX_SIZE_UDP);
	if ( (_ImageSize % MAX_SIZE_UDP) != 0) _ImageParts++;

	LOGGER_DEBUG("Number of image's parts: " << _ImageParts);

	// Sends image informations to Server
	BYTE datas[] = { SEND_IMAGE_INFO ,(uint8_t)((_ImageParts >> 8)&0xFF ),(uint8_t)( _ImageParts & 0xFF) };
	Udp::_udp->send(datas,3,_TcpInfo->ipAddress);

	// Send All Parts
	for (int i = 0; i < _ImageParts; i++)
	{
		REC_TO_SRV_ImagePart( i );
	}

	// Send Flag to indicate all parts have been sent
	REC_TO_SRV_AckImage();
}

/** REC_TO_SRV_TcpAck
 * Send a Ack to Server
 **/
void REC_TO_SRV_TcpAck( BYTE command )
{
	BYTE d[] = { ACK_TCP , command };
	Tcp::_tcp->sendData(d,2,false);
}

/** REC_TO_SRV_StorageAsk
 * Send a Request to send a file In Tcp
 **/
void REC_TO_SRV_StorageAsk( unsigned long fileInQueue )
{
	int size = fileInQueue / 256 + 1;
	BYTE res[size + 1];
	res[0] = SEND_STORAGE_REQUEST;
	for ( int i = 0; i < size; i++ )
	{
		res[i+1] = ( fileInQueue >> (8*(size-i-1))) & 0xFF ;
	}
	LOGGER_VERB("Send Storage Ask : " << fileInQueue );
	Tcp::_tcp->sendData(res,size+1,false);
}

/** REC_TO_SRV_EndFileTransfert
 * Send to Server Frame to signal the end of a File Upload
 **/
void REC_TO_SRV_EndFileTransfert( string chksum, char* fileName, BYTE nameLength )
{
	int size = 2 + nameLength + chksum.length();
	BYTE res [ size ];
	res[0] = SEND_FILE_SEND_TCP;
	res[1] = nameLength; // SizeOf Name
	memcpy((res+2), fileName,nameLength);
	memcpy((res+2+nameLength),(char*)chksum.c_str(),chksum.length());
	Tcp::_tcp->sendData(res, size ,false);
}

/** REC_TO_SRV_Authenification
 * Send to Server Tag Id to validate Card
 **/
void REC_TO_SRV_Authenification( uint64_t idTag )
{
	LOGGER_VERB("Send to SRV authentification Request ");
	BYTE res[] =
	{
		SEND_AUTHENTIICATION_REQUEST_TCP,
		(uint8_t) (( idTag >> 56  )& 0xFF),
		(uint8_t) (( idTag >> 48 ) & 0xFF),
		(uint8_t) (( idTag >> 40 ) & 0xFF),
		(uint8_t) (( idTag >> 32 ) & 0xFF),
		(uint8_t) (( idTag >> 24 ) & 0xFF),
		(uint8_t) (( idTag >> 16 ) & 0xFF),
		(uint8_t) (( idTag >> 8 )  & 0xFF),
		(uint8_t) (  idTag         & 0xFF)
	};
	Tcp::_tcp->sendData(res, 9 ,false);
}

/** REC_TO_ACT_AuthenificationAns
 * Send to Act Authentication answer
 **/
void REC_TO_ACT_AuthenificationAns( bool authorisation )
{
	LOGGER_VERB("Ans Anthentification to NRF : " << authorisation );
	BYTE answer[] = { AUTH_ANS, ( authorisation )?(uint8_t)0x01:(uint8_t)0x00 } ;
	Nrf24::_Nrf->sendBuffer(AUTH_ANS,answer,2,false, 0);
}

/** REC_TO_SRV_IdRecording
 * Send to SRV to get recording Id
 **/
void REC_TO_SRV_IdRecording()
{
	BYTE res[] = { ASK_ID_RECORDING };
	Tcp::_tcp->sendData(res, 1 ,false);
}

/** REC_TO_SRV_RECORDING_END
 * Send To SRV the end OF Records
 **/
void REC_TO_SRV_RECORDING_END(uint64_t idRecording)
{
	LOGGER_VERB("SEND RECORDING END " << idRecording );
	BYTE res[] =
	{
		SEND_RecordingEnd,
		(uint8_t) (( idRecording >> 56  )& 0xFF),
		(uint8_t) (( idRecording >> 48 ) & 0xFF),
		(uint8_t) (( idRecording >> 40 ) & 0xFF),
		(uint8_t) (( idRecording >> 32 ) & 0xFF),
		(uint8_t) (( idRecording >> 24 ) & 0xFF),
		(uint8_t) (( idRecording >> 16 ) & 0xFF),
		(uint8_t) (( idRecording >> 8 )  & 0xFF),
		(uint8_t) (  idRecording         & 0xFF)
	};
	Tcp::_tcp->sendData(res, 9 ,false);
}

/** REC_TO_SRV_ParringStatus
 * Send To SRV the state of Parring
 **/
void REC_TO_SRV_ParringStatus( bool succes )
{
	BYTE res[] = { ANS_PARIRING , ( succes == true ) ?(BYTE) 0x01 :(BYTE) 0x00 } ;
	Tcp::_tcp->sendData(res, 2 ,true);
}




/******** -------------------------- 	NRF CALLBACKS -------------------------- ********/

/** ACT_TO_REC_ACK
 * Acknowledged received from Nrf
 **/
void ACT_TO_REC_ACK(BYTE data[], int size)
{
	if ( size == 0 )
		return;

	LOGGER_DEBUG("Nrf ACK " << (int) data[0]);
	if ( Nrf24::_Nrf->_FrameToSend.empty() )
		return;

	Frame f = Nrf24::_Nrf->_FrameToSend.front();
	if ( f.header == data[0] )
		Nrf24::_Nrf->_FrameToSend.pop_front();
}

/** ACT_TO_REC_Authentification
 * Callback When a Tag is put on RIFD Reader
 **/
void ACT_TO_REC_Authentification( BYTE data[], int size )
{
	// Refused request if Nrf in is Parking Mode
	if ( isParking == true || Nrf24::_Nrf->_State != WAIT_FOR_DATA )
		return;

	uint64_t idTag = 0;
	for(int i = 0; i < size; i++) idTag = ( idTag << 8 ) | data[i];

	LOGGER_INFO("Authentication request. (Tag Id: " << idTag <<")");

	// Send IdTag to Srv to validation
	REC_TO_SRV_Authenification( idTag );

}

/** ACT_TO_REC_StatusRequest
 *	CallBack when receive a demand of status from NRF
 **/
void ACT_TO_REC_StatusRequest( BYTE data[], int size )
{
	LOGGER_DEBUG("Status Nrf request received");
	if ( isParking == true )
	{
		LOGGER_WARN("Module in Parking => Status request ignored");
		return;
	}

	// TODO : When size can have a size of -1 ???
	if ( size == -1 )
	{
		BYTE answer[] = {  STATUS_ASK, ( isRecording )?(uint8_t)0x01:(uint8_t)0x00 } ;
		Nrf24::_Nrf->sendBuffer(STATUS_ASK, answer, sizeof(answer), false, 0);
		return;
	}

	// Send to Nrf the status of this module
	BYTE answer[] = {  STATUS_ANS, ( isRecording )?(uint8_t)0x01 : (uint8_t)0x00 } ;
	Nrf24::_Nrf->sendBuffer(STATUS_ANS, answer, sizeof(answer), true, 0);
}

/** ACT_TO_REC_StatusAns
 * 	CallBack when Receive status from Nrf
 **/
void ACT_TO_REC_StatusAns( BYTE data[], int size )
{
	LOGGER_DEBUG("Status Nrf Ans received");
	if ( isParking == true )
	{
		LOGGER_WARN("Module in Parking => Status Ans ignored" );
		return;
	}
	// TODO : Why The delete Frame is here ?
	if ( ! Nrf24::_Nrf->_FrameToSend.empty()  )
	{
		Frame f =  Nrf24::_Nrf->_FrameToSend.front();
		if ( f.header == STATUS_ASK )
		{
			Nrf24::_Nrf->_FrameToSend.pop_front();
			Nrf24::_Nrf->_State = WAIT_FOR_DATA;
		}
	}
	Nrf24::_Nrf->sendAck(STATUS_ANS);
}

/** ACT_TO_REC_ControlRequest
 * 	CallBack when Nrf ask to start/stop a recording
 **/
void ACT_TO_REC_ControlRequest( BYTE data[], int size )
{
	if(size == 0)
		return;
	LOGGER_DEBUG("Control request received : " << (int)data[0]);
	if ( isParking == true || Nrf24::_Nrf->_State != WAIT_FOR_DATA )
	{
		LOGGER_WARN("Module in Parking OR Waiting for data => Control request ignored");
		return;
	}

	Nrf24::_Nrf->sendAck(ACTIVATE_RECORDING);
	if ( data[0] == 0x01 )
	{
		// Start Recording
		if ( isRecording )
		{
			LOGGER_WARN("Module in already in RecordMode. Ignore request");
			return;
		}
		if ( _IdUserRecorder == 0x00 )
		{
			LOGGER_WARN("No Tag Presented. Ignore request");
			return;
		}
		REC_TO_SRV_IdRecording();
	}
	else
	{
		// Stop Recording
		if ( ! isRecording )
		{
			LOGGER_WARN("Module isn't in RecordMode. Ignore request");
			return;
		}
		stopRecording();
	}
}

/** ACT_TO_REC_PoolingParkingAns
 * 	Callback when ??
 **/
void ACT_TO_REC_PoolingParkingAns(BYTE data[], int size)
{
	if ( isParking == false )
	{
		return;
	}
	// TODO: N'authoriser qu'une suele réponse par demande d'appairage
	Nrf24::_Nrf->clearCommunications();
	//LOGGER_DEBUG("Appairage ID recieved :" << std::uppercase << std::hex << data[0] << " " << std::uppercase << std::hex<< data[1]);

	memcpy(idGenerate,data,2);

	BYTE answer[] = {  	POOL_SET_ADDR , idGenerate[0] , idGenerate[1] ,
						(uint8_t)((addressGenerate[0]>>24) & 0xFF), (uint8_t)((addressGenerate[0]>>16) & 0xFF),  (uint8_t)((addressGenerate[0]>>8) & 0xFF),  (uint8_t)((addressGenerate[0] & 0xFF)),
						(uint8_t)((addressGenerate[1]>>24) & 0xFF) ,(uint8_t)((addressGenerate[1]>>16) & 0xFF),  (uint8_t)((addressGenerate[1]>>8) & 0xFF),  (uint8_t)((addressGenerate[1] & 0xFF)),
					} ;
	Nrf24::_Nrf->sendBuffer(POOL_SET_ADDR, answer ,sizeof(answer) , false, 5);
}

/** ACT_TO_REC_ValidateParringConfig
 * 	CallBack when Nrf validate configuration
 **/
void ACT_TO_REC_ValidateParringConfig(BYTE data[], int size)
{
	if ( isParking == false )
	{
		return;
	}
	isParking = false;
	Nrf24::_Nrf->clearCommunications();

	Nrf24::_Nrf->sendAck(POOL_VALIDATE_CONFIG);
	// Wait for ack will send
	usleep(2500000);
	// Configure the new Addresses
	Nrf24::_Nrf->updateAddr( addressGenerate[0], addressGenerate[1], true );
	REC_TO_SRV_ParringStatus(true);
}






/******** -------------------------- 	UDP CALLBACKS -------------------------- ********/

/** SRV_TO_REC_InfoSrvAns
 * 	CallBack when server send his Configuration
 **/
void SRV_TO_REC_InfoSrvAns ( BYTE data[], int size )
{
	if ( size != 4 )
		return;
	sprintf( _TcpInfo->ipAddress ,"%s",Udp::_udp->_IpSender);
	_TcpInfo->port = data[0]<<8 | data[1] ;
	_FtpPort = data[2]<<8 | data[3] ;

	_FtpHost = SSTR ( _TcpInfo->ipAddress << ":" << _FtpPort );
	LOGGER_DEBUG( "Detect : Udp Port: " << _TcpInfo << " FTP: " << _FtpPort ) ;

	BYTE ans[2];
	ans[0] = ACK_UDP;
	ans[1] = GET_INFO_SRV;

	Udp::_udp->send(  ans, 2 , _TcpInfo->ipAddress);
}

/** SRV_TO_REC_AckImage
 * 	CallBack when Seveur send a image ack
 **/
void SRV_TO_REC_AckImage ( BYTE data[], int size )
{
	_WaitAckImage = false;

	for ( int i = 0 ; i < size; i+=2 )
	{
		REC_TO_SRV_ImagePart(data[i]<<8 | data[i+1]  );
	}
	if ( size != 0 )
	{
		REC_TO_SRV_AckImage();
	}
	else
	{
		// Image completly received ==> Clear buffer
		LOGGER_VERB("Image completely received");
		free( _ImageContent );
		_ImageContent = NULL;
		_ImageParts = 0;
		_ImageSize = 0;
	}
}





/******** -------------------------- 	TCP CALLBACKS -------------------------- ********/

/** SRV_TO_REC_VisuCam
 * 	Callback when Server ask to get Image from Camera
 **/
void SRV_TO_REC_VisuCam( BYTE* data, unsigned long size )
{
	// Ignore: No Camera Id Pass
	if ( size != 1 )
		return;

	REC_TO_SRV_Image( data[0] );
}

/** SRV_TO_REC_ParringAsk
 * 	CallBack when Server send The Nrf Address
 **/
void SRV_TO_REC_ParringAsk( BYTE* data, unsigned long size )
{
	if ( size != 8 )
		return;

	// TODO: UPDATE NRF STATUT
	addressGenerate[0] = (uint64_t) (data[0] << 24) |  (uint64_t) data[1] << 16 | (uint64_t) data[2] << 8 | (uint64_t) data[3];
	addressGenerate[1] = (uint64_t) (data[4] << 24) |  (uint64_t) data[5] << 16 | (uint64_t) data[6] << 8 | (uint64_t) data[7];

	REC_TO_SRV_TcpAck( ASK_PARIRING );
	ParringNrf();
}

/** SRV_TO_REC_Authentification
 * 	Callback from Server to validate a TagId
 **/
void SRV_TO_REC_Authentification( BYTE* data, unsigned long size )
{
	if ( size == 0 ) // Can't have no data
		return;

	if ( isRecording )
	{
		LOGGER_WARN("Authentication ignore : Currently Recording");
		return;
	}
	if ( data[0] == 0x00 )
	{
		LOGGER_DEBUG ( "GetAuthentification: Tag unknown" ) ;
		REC_TO_ACT_AuthenificationAns( false );
	}
	else if ( data[0] == 0x01 && size > 1 )
	{
		// Save idUserRecorder
		_IdUserRecorder = 0;
		for (unsigned long i = 1; i < size ; i++ ) _IdUserRecorder = (_IdUserRecorder << 8) | data[i];
		REC_TO_ACT_AuthenificationAns( true );
	}
	REC_TO_SRV_TcpAck(GET_AUTHENTIFICATION_ANS_TCP);
}

/** SRV_TO_REC_StatutRequest
 * 	CallBack when Server ask for Status
 **/
void SRV_TO_REC_StatutRequest( BYTE* data, unsigned long size )
{
	BYTE res[3];
	res[0] = ANS_STATUS_TCP;
	res[1] = _StateArduino;
	res[2] = _StateRaspberry;
	// Send status
	Tcp::_tcp->sendData(res,3,true);
}

/** SRV_TO_REC_StorageAns
 * 	CallBack from Server to validate if this module can send a file in FTP
 **/
void SRV_TO_REC_StorageAns( BYTE* data, unsigned long size )
{
	LOGGER_VERB("Get Storage Ans");
	if ( size <= 1 ) // Can't has no data
		return;
	_FtpPassword = "";
	for ( unsigned long i = 1; i < size ; i++ )
		_FtpPassword = SSTR ( _FtpPassword << data[i] );

	_FtpCanUploadFile = ( data[0] == 0x01 );
	REC_TO_SRV_TcpAck(GET_STORAGE_ANS);
/*	if ( data[0] == 0x01 )
	{
		// TODO : Upload One File
		LOGGER_DEBUG ( "Can upload one file" ) ;
		//ftpSendFile();
	}*/
	_FtpWaitStorageAns = false;
}

/** SRV_TO_REC_FileTransfertAck
 * 	CallBack from Server to validate a file transfer
 **/
void SRV_TO_REC_FileTransfertAck( BYTE* data, unsigned long size )
{
	if ( size != 1 )  // Can't has no data
		return;

	if ( !fileInUpload || fileInUpload->isInUpload )
	{
		// EROR No file Was Send
		return;
	}

	if ( data[0] == 0x01 )
	{
		// TODO : Upload OK :: DELETE FILE FROM RPI
		//LOGGER_INFO ( "File Upload OK" ) ;
		fileInUpload->isUploaded = true;
	}
	else
	{
		// TODO : RESEND FILE
		LOGGER_ERROR( "Error during File Transfer ");
	}
	_FtpGetImageOk = ( data[0] == 0x01 );
	_FtpWaitAckMd5 = false;
	REC_TO_SRV_TcpAck(ACK_FILE_SEND_TCP);
}

/** SRV_TO_REC_GetIdRecording
 * 	CallBack from Server when Generate Recording Id
 **/
void SRV_TO_REC_GetIdRecording( BYTE* data, unsigned long size)
{
	if ( size != 8 )
		return;

	uint64_t idRecording = 	(uint64_t) data[0] << 56 | (uint64_t) data[1] << 48 | (uint64_t) data[2] << 40 |(uint64_t)
							data[3] << 32 |( uint64_t) data[4] << 24 | (uint64_t)  data[5] << 16 |(uint64_t) data[6] << 8 |
							(uint64_t)  data[7];
	startRecording( idRecording );
}





/******** -------------------------- 	Recordings Functions -------------------------- ********/
bool startRecording( uint64_t idRecording )
{
	_CurrentRecording = new Recording( idRecording );
	isRecording = _CurrentRecording->startRecord();
	_CurrentRecording->makeDirectory();
	_IdRecording = idRecording;


	if(isRecording && Webcam::isInUse() && StillCamera::isInUse())
	{
		LOGGER_VERB("Will start cameras");
		StillCamera * still = StillCamera::getCamera();
		Webcam * cam = Webcam::getWebcam();
		SoundRecord * sound = SoundRecord::getSoundRecord();
		if(!cam->isRecording() && !still->isRecording() && !sound->isRecording())
		{
			LOGGER_VERB("Starting Cameras and sound recording");
			isRecording &= cam->startRecording(_CurrentRecording);
			#ifndef DEBUG_IMAGE
            isRecording &= still->startRecording(_CurrentRecording);
			isRecording &= sound->startRecording(_CurrentRecording);
			#endif

		}
		else
			LOGGER_WARN("At least one camera is already in recording");
	}
	else
		LOGGER_WARN("Recording not started : Webcam : "<< Webcam::isInUse() <<" Camera : " << StillCamera::isInUse());
	return isRecording;
}

bool stopRecording()
{
	if(Webcam::isInUse())
	{
		Webcam * cam = Webcam::getWebcam();
		if(cam->isRecording())
			cam->stopRecording();
	}
	if(StillCamera::isInUse())
	{
		StillCamera * cam = StillCamera::getCamera();
		if(cam->isRecording())
			cam->stopRecording();
	}
	SoundRecord * sound = SoundRecord::getSoundRecord();


	if(sound != NULL && sound->isRecording())
		sound->stopRecording();
	if(_CurrentRecording != NULL)
	{
		_CurrentRecording->stopRecord();
		_CurrentRecording = NULL;
		REC_TO_SRV_RECORDING_END( _IdRecording );
		isRecording = false;
	}

	_CurrentRecording = NULL;
	_IdUserRecorder = 0x00;
	_IdRecording = 0x00;

	return true;
}

/******** -------------------------- 	Camera Functions -------------------------- ********/

/** getImageFromCam
 * 	Load from Video calls an image
 **/
bool getImageFromCam ( BYTE idCam )
{

	/*streampos size;
	ifstream file ("img.jpg", ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		_ImageSize = file.tellg();
		if ( _ImageContent != NULL )
			free(_ImageContent);

		_ImageContent = new char [_ImageSize];
		file.seekg (0, ios::beg);
		file.read (_ImageContent, _ImageSize);
		file.close();

		LOGGER_VERB("Image load");
	}
	else{
		LOGGER_WARN("Unable to open file");
		return false;
	}*/
	if(StillCamera::isInUse())
	{
		StillCamera * cam = StillCamera::getCamera();
		if(cam->isRecording())
		{
			LOGGER_WARN("Cannot get image while in recording !");
			return false;
		}
		if ( _ImageContent != NULL )
			free(_ImageContent);
		return cam->takeSnapshot((unsigned char**)&_ImageContent,(int *)(&_ImageSize));
	}
	return false;
}


/******** -------------------------- 	FTP Functions -------------------------- ********/
bool ftpCheck( )
{
	if ( !_Ftp->Connect(_FtpHost.c_str() ))
	{
		LOGGER_ERROR("Failed to FTP Connect. Host error: " << _FtpHost.c_str()  );
		return false;
	}

	if ( !_Ftp->Login((char * )FTP_TEST_USER , (char*) FTP_TEST_PASSWORD ))
	{
		LOGGER_ERROR("Failed to FTP Connect. User/Password error");
		return false;
	}
	LOGGER_DEBUG("FTP Connection OK");
	_Ftp->Quit();

	return true;
}

void ftpSenderStart()
{
	pthread_create( &_FtpThread , NULL ,  &ftpSenderThread , NULL );
}

bool ftpSendFile( )
{
	if ( fileInUpload == NULL )
		return false;

	_isSending = true;
	fileInUpload->isInUpload = true;
	LOGGER_DEBUG( "Send File : " << fileInUpload->fileName << "::" << fileInUpload->idRecording );
	_FtpSendFileName = SSTR ( "/" << fileInUpload->idRecording << "/"  << fileInUpload->fileName );
	if ( !_Ftp->Connect( _FtpHost.c_str() ) )
	{
		LOGGER_ERROR("Failed to FTP Connect. Host error");
		return false;
	}

	if ( !_Ftp->Login(_FtpUser.c_str() , _FtpPassword.c_str() ) )
	{
		LOGGER_ERROR("Failed to FTP Connect. User/Password error. " << _FtpUser << ":" << _FtpPassword );
		return false;
	}

	_Ftp->SetConnmode(ftplib::pasv);
	/*int mkdirRes = */_Ftp->Mkdir( SSTR("/" << fileInUpload->idRecording << "/").c_str());
	int uploadRes = _Ftp->Put( fileInUpload->path.c_str() ,(char *)_FtpSendFileName.c_str(), ftplib::image );
	if ( uploadRes == 1 ){
		int size = _FtpSendFileName.length();
		REC_TO_SRV_EndFileTransfert( fileInUpload->chks ,(char *) _FtpSendFileName.c_str() , (BYTE ) size  );
	}
	cout << "Up : "<<uploadRes <<endl;
	fileInUpload->isInUpload = false;
	_Ftp->Quit();
	_isSending = false;
	return ( uploadRes == 0x01 );
}

void* ftpSenderThread( void* data )
{
	LOGGER_DEBUG("Ftp Client Start : Start Sender Thread ");

	/** Register signal handler for SIGPIPE **/
	signal(SIGPIPE, sigpipe_handler);
	sleep(15);
	fileInUpload = Recording::getNextFile();
	Recording *r = NULL;
	while ( true )
	{
		if ( ! fileInUpload ) {
			fileInUpload = Recording::getNextFile();
			if ( !fileInUpload )
				sleep( 60 );
			continue;
		}
		fileInUpload->generateChksum();
		LOGGER_VERB("SEND TCP REQUEST");
		_FtpWaitStorageAns = true;
		_FtpCanUploadFile = false;
		_FtpWaitAckMd5 = true;

		REC_TO_SRV_StorageAsk( Recording::_FilesNotUpload );

		while ( _FtpWaitStorageAns == true ) usleep(10000);

		if ( ! _FtpCanUploadFile ) {
			sleep(60);
			continue;
		}
		if ( ftpSendFile() ){
			while ( _FtpWaitAckMd5 == true ) usleep( 10000 );
			if ( _FtpGetImageOk )  {
				LOGGER_INFO("Ftp: UPLOAD FILE OK");
				fileInUpload->isUploaded = true;
				r = fileInUpload->recording;
				fileInUpload->uploadOk();
				fileInUpload = NULL;
				if ( !r->isRecording() ){
					cout << "Is Not Recording " << endl;
					if ( r->_Files.empty() ){
						REC_TO_SRV_RECORDING_END( r->_IdRecording );
						rmdir ( (char*) r->_folderRecording.c_str() );
						Recording::_Recordings.remove( r );
						r = NULL;
					}
				}
			}
		}
		sleep(1);
	}
	return NULL;
}
