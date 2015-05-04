#define IMAGE_SIZE_MAX 2000
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "define.h"
#include "Recorder.h"
#include "Communication.h"
#include "Mysql.h"


list<Recorder *> Recorder::_Recorders;
using namespace std;

/****************  Constructor  ****************/ 
Recorder::Recorder ( Udp *udp, char* ip ){
	// Init Variable
	this->_StatutRasp = 0x00;
	this->_StatutArd  = 0x00;
	this->_IdRecorder = 0;
	this->_IdUserRecording = 0;
	this->_UdpSrv = udp;
	this->_UdpSocket = NULL;
	this->_ImageParts = NULL;
	this->_Tcp = NULL;

	this->_IpAddr = ( char *) malloc ( strlen(ip) );
	memcpy( _IpAddr,ip,strlen(ip) );

	// Configure Udp callbacks
	this->_CommunicationUdp = new Communication( 0x10, 0x22, 0x20, IMAGE_SIZE_MAX ); 
	_CommunicationUdp->setFunction((FuncType)&(this->REC_TO_SRV_ImageInfo), (int)GET_IMAGE_INFO);
	_CommunicationUdp->setFunction((FuncType)&(this->REC_TO_SRV_ImagePart), (int)GET_IMAGE_PARTS);
	_CommunicationUdp->setFunction((FuncType)&(this->REC_TO_SRV_getMacAddress), (int)GET_MAC_ADDR);
	_CommunicationUdp->setFunction((FuncType)&(this->REC_TO_SRV_imageCompletlySend), (int)GET_IMAGE_COMPLETLY_SEND);

	// Add recorder to List
	_Recorders.push_back(this);
}





/****************  Getters / Setters  ****************/ 
Recorder* Recorder::findRecorderByIp( char* ip ){
	if ( _Recorders.empty() )
		return NULL;

	for ( list<Recorder*>::iterator it = _Recorders.begin(); it != _Recorders.end(); ++it ){
		Recorder *rec = *it;
		if ( strcmp(ip,rec->_IpAddr) == 0 )
			return rec;
	}
	return NULL;
}  

bool Recorder::isTcpConnected(){
	return ( this->_Tcp != NULL );
}

/** setTcpSocket
 * Initialise TCP CallBacks
 **/
void Recorder::setTcpSocket( Tcp* s ){
	this->_Tcp = s;
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_getStatut), (int)GET_STATUT);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_authentificationAsk), (int)GET_AUTHENTIFICATION_REQ);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_ansParring), (int)ANS_ASK_APPARAIGE);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_askSendFile), (int)ASK_STORAGE);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_endFileTransfert), (int)GET_END_FILE_TRANSFERT);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_endFilesTransfert), (int)GET_END_TRANSFERT_FILES);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_recordingEnd), (int)GET_RecordingEnd);
	_Tcp->_Communication->setFunction((FuncType)&(this->REC_TO_SRV_createRecording), (int)GET_RECORDING_START );
	SRV_TO_REC_askVisuImage(0x01);
}

void Recorder::setUdpSocket ( void* sock, int size ){
	if ( this->_UdpSocket != NULL )
		free ( this->_UdpSocket );

	this->_UdpSocket = malloc( size );
	memcpy( this->_UdpSocket , sock, size );

}





/****************  Communications functions  ****************/ 
void Recorder::getFrameUdp( BYTE* data, unsigned long size  ){
	if ( _CommunicationUdp == NULL )
		return;
	//for (unsigned long i = 0; i < size; i++ ) printf("%02x ", data[i] );
	_CommunicationUdp->recieveData(data, size, (void*) this);
}

bool Recorder::sendUdpFrame( BYTE* data, int size ){
	return this->_UdpSrv->sendFrame(_UdpSocket, data, size);
}

bool Recorder::sendTcpFrame( BYTE* data , int size, bool needAck){
	if ( isTcpConnected() == false )
		return false;
	LOGGER_VERB("Send Data To TCP");
	_Tcp->sendTcp( data , size, needAck );
	return true;
}






/****************  Request  ****************/

/** SRV_TO_REC_askVisuImage
 * Send to Recorder a request to get an image from Camera
 **/
void Recorder::SRV_TO_REC_askVisuImage( BYTE idCam ){
	BYTE req[] = { ASK_VISU_CAM , idCam } ; 
	sendTcpFrame(req,2,false);
}

/** SVR_TO_REC_askStatut
 * Ask to recorder his status
 **/
void Recorder::SRV_TO_REC_askStatut(){
	BYTE req[] = { SEND_ASK_STATUT } ; 
	sendTcpFrame(req,1,false);
}


/** SRV_TO_REC_sendParring
 * Send Nrf Address to Recorder
 **/
void Recorder::SRV_TO_REC_sendParring( ){
	// TODO : GENERATE WITH DATABASE ON 32Bits each One

	LOGGER_DEBUG("Send NRF ADDRESSES TO Recorder");
	uint64_t addressGenerateRpi =  0x00;
	uint64_t addressGenerateArd =  0x00;

	if ( ! Mysql::generateNrfAddress( _IdRecorder , &addressGenerateRpi, &addressGenerateArd )){
		LOGGER_ERROR("Cannot generate NRF ADDRESSES");
		return;
	}
	BYTE req[] = { SEND_ASK_PAIRING , 
		(uint8_t) (( addressGenerateRpi >> 24 ) & 0xFF),
		(uint8_t) (( addressGenerateRpi >> 16 ) & 0xFF),
		(uint8_t) (( addressGenerateRpi >> 8  ) & 0xFF),
		(uint8_t) (( addressGenerateRpi       ) & 0xFF),
		(uint8_t) (( addressGenerateArd >> 24 ) & 0xFF),
		(uint8_t) (( addressGenerateArd >> 16 ) & 0xFF),
		(uint8_t) (( addressGenerateArd >> 8  ) & 0xFF),
		(uint8_t) (  addressGenerateArd 	& 0xFF)
	};
	sendTcpFrame(req, 9 ,true);
}




/****************  Udp Callbacks  ****************/
/** REC_TO_SRV_getMacAddress
 * Get the broadcast Frame from Recorder
 **/
void Recorder::REC_TO_SRV_getMacAddress( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;

	memcpy( rec->_MacAddress, data , size );

	rec->_IdRecorder = Mysql::getIdRecorderFromMac( SSTR(rec->_MacAddress).substr(0,12) );

	if ( rec->_IdRecorder == 0 ){
		LOGGER_WARN("Recorder Unkown : " << rec->_MacAddress << ". Ignore Request");
		return;
	}

	BYTE req[] = { 0x42 , (BYTE)(CurrentApplicationConfig.TCP_serverPort >> 8),
		(BYTE)(CurrentApplicationConfig.TCP_serverPort & 0xFF),
		(BYTE)(CurrentApplicationConfig.FTP_serverPort >> 8), 
		(BYTE)(CurrentApplicationConfig.FTP_serverPort & 0xFF) };
	BYTE* ans = NULL;

	int t = rec->_CommunicationUdp->encodeData(req,&ans, sizeof(req));
	rec->sendUdpFrame(ans, t);
}

/** REC_TO_SRV_ImageInfo
 * Get the number of parts for image
 **/
void Recorder::REC_TO_SRV_ImageInfo( BYTE* data, unsigned long size, void *sender ){
	if ( size != 2 )
		return;

	int ss = data[0] <<8 | data[1];
	LOGGER_DEBUG( " Get Image info. nbParts: " << ss);

	Recorder *rec = (Recorder*) sender;

	// Free lasted image
	if ( rec->_Image != NULL ){
		for (int i =0; i < rec->_SizeImage ;i++){
			if ( rec->_Image[i] != NULL ){
				free(rec->_Image[i]);
				rec->_Image[i] = NULL;
			}
		}
		free(rec->_Image);
		rec->_Image = NULL;
	}

	if ( rec->_ImageParts != NULL ){
		free ( rec->_ImageParts );
		rec->_ImageParts = NULL;
	}

	// Allocate spaces for images
	rec->_ImageParts = ( int * ) malloc ( sizeof(int) * ss);

	memset(rec->_ImageParts,0,ss);

	rec->_Image = ( char**) malloc ( sizeof(char*) * ss );
	memset(rec->_Image,0,sizeof(char*) * ss);
	rec->_SizeImage = ss;
	rec->_Count = 0;
}

/** REC_TO_SRV_ImagePart
 * Get a part of Image
 **/
void Recorder::REC_TO_SRV_ImagePart( BYTE* data, unsigned long size, void *sender ){
	if ( size < 3 )
		return;

	Recorder *rec = (Recorder*) sender;
	if ( rec->_Image == NULL ) return;

	int partNo = data[0]<<8 | data[1];
	rec->_ImageParts[ partNo ] = size - 2;

	rec->_Count++;
	// cout << "Get Image Parts: " << partNo << " ==> " << rec->_Count << endl;

	if ( rec->_Image[partNo] == NULL ){
		rec->_Image[partNo] = ( char* ) malloc (sizeof(char)*size-2);
		memcpy(rec->_Image[partNo],data+2,size-2);
	}
}

/** REC_TO_SRV_imageCompletlySend
 * Get From Recorder a signal to all images parts will be send
 **/
void Recorder::REC_TO_SRV_imageCompletlySend( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;
	list<int> l;

	//cout << "Get Completly " << endl;
	for (int i = 0;  i < rec->_SizeImage ;i++){
		if ( rec->_ImageParts[i] == 0 ){
			l.push_back(i);
		}
		if ( l.size() > 150 )
			break;
	}
	int siz = 1 + l.size() * 2 ; 
	BYTE* req = new BYTE[ 1 + l.size() * 2];
	req[0] = SEND_PARTS_NOT_RECIEVED ;
	int i = 1;


	if ( l.size() == 0 ) {
		LOGGER_DEBUG("Image completly Recieve");
		// Y a toute l'image
		ofstream myFile ("img.jpg", ios::out | ios::binary);
		for (int i =0; i < rec->_SizeImage ;i++){
			myFile.write (rec->_Image[i], rec->_ImageParts[ i ] );
		}
		myFile.close();
	}


	while ( l.size() != 0 ){
		int d = l.front();
		l.pop_front();
		req[i++] = d >> 8 & 0xFF;
		req[i++] = d & 0xFF;
		// cout << "Request for " << d << endl;
	}

	// Send idParts not recieve
	BYTE* ans = NULL;
	int t = rec->_CommunicationUdp->encodeData(req, &ans, siz );
	usleep(100000);
	rec->sendUdpFrame(ans, t);
}




/****************  TCP CallBacks  ****************/ 

/** REC_TO_SRV_getSatut
 * Recieve Status from Recorder
 **/
void Recorder::REC_TO_SRV_getStatut( BYTE* data, unsigned long size, void *sender ){
	if ( size != 2 )
		return;
	Recorder *rec = (Recorder*) sender;
	LOGGER_DEBUG("Get Status : " << data[0] << ":" << data[1] );
	rec->_StatutRasp = data[0];
	rec->_StatutArd  = data[1];
}

/** REC_TO_SRV_authentificationAsk
 * Recieve a Tag id from Recorder to verify UserRecorder
 **/
void Recorder::REC_TO_SRV_authentificationAsk( BYTE* data, unsigned long size, void *sender ){
	if ( size != 8 ){
		return;
	}

	Recorder *rec = (Recorder*)sender;
	if ( rec->_IdRecorder == 0 ){
		LOGGER_WARN("Authentification request ignore. Recorder unkown");
		return;
	}

	uint64_t idCard = (uint64_t) ( (uint64_t) data[0] << 56 | (uint64_t) data[1] << 48 | (uint64_t )data[2] << 40 | (uint64_t) data[3] << 32 |
			(uint64_t) data[4] << 24 | (uint64_t) data[5] << 16 | (uint64_t) data[6] << 8  | (uint64_t) data[7]);

	LOGGER_DEBUG("Search  authentification for: " << idCard );				  
	rec->_IdUserRecording = Mysql::getIdUserRecorderFromTag( idCard );

	if ( rec->_IdUserRecording != 0x00 ){
		BYTE req[] = { ANS_AUTHENTIFICATION , 0x01, 
			(uint8_t) (( rec->_IdUserRecording >> 56 ) & 0xFF),
			(uint8_t) (( rec->_IdUserRecording >> 48 ) & 0xFF),
			(uint8_t) (( rec->_IdUserRecording >> 40 ) & 0xFF),
			(uint8_t) (( rec->_IdUserRecording >> 32 ) & 0xFF),
			(uint8_t) (( rec->_IdUserRecording >> 24 ) & 0xFF),
			(uint8_t) (( rec->_IdUserRecording >> 16 ) & 0xFF),
			(uint8_t) (( rec->_IdUserRecording >> 8 )  & 0xFF),
			(uint8_t) (  rec->_IdUserRecording 	  & 0xFF)
		};
		rec->sendTcpFrame(req,10,true);
	}
	else{
		LOGGER_WARN("Authentification Error : Tag unkown or invalid");
		BYTE req[] = { ANS_AUTHENTIFICATION , 0x00 };
		rec->sendTcpFrame(req,2,true);
	}
}

/** REC_TO_SRV_ansParring
 * Get parring statut
 **/
void Recorder::REC_TO_SRV_ansParring( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;

	if ( size != 1 || rec->_IdRecorder == 0 )
		return;

	if ( data[0] == 0x01 ){
		LOGGER_INFO("Parraing OK");
	}
	else{
		LOGGER_ERROR("Parraing FAILED");
	}
	rec->_Tcp->sendAck( (BYTE*) ANS_ASK_APPARAIGE );
}

/** REC_TO_SRV_askSendFile
 * Recieve a ask to send file with FTP
 **/
void Recorder::REC_TO_SRV_askSendFile( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;
	if ( rec->_IdRecorder == 0 )
		return;

	long fileInWait = 0;
	for ( unsigned long  i = 0; i < size; i++ ) fileInWait = ( fileInWait << 8) | data[i];
	LOGGER_DEBUG( "Storage Ask from: " << rec->_IdRecorder << " Files in wait: " << fileInWait );

	// TODO : Verifiy if can send File
	bool canSend = true;

	if ( canSend ){
		LOGGER_DEBUG("Can Send File");
		BYTE req[2]; 
		req[0] = ANS_STORAGE;
		req[1] = 0x01;
		rec->sendTcpFrame(req,2,true);
	}
	else{
		LOGGER_DEBUG("Can't send file" );
		BYTE req[1]; 
		req[0] = ANS_STORAGE;
		req[2] = 0x00;
		rec->sendTcpFrame(req,2,true);
	}
}

/** REC_TO_SRV_endFileTransfert
 * Recieve a signal to end file transfert
 **/
void Recorder::REC_TO_SRV_endFileTransfert( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;
	if ( rec->_IdRecorder == 0 )
		return;

	LOGGER_DEBUG("Get endFileTransfert CompareChksSum: ") ;

	// TODO : CompareChks
	bool sameChkSum = true;

	BYTE req[2]; 
	req[0] = ANS_END_FILE_TRANSFERT;

	if ( sameChkSum ){
		LOGGER_INFO( "File OK");
		// TODO: ADD FILE TO BDD
		req[1] = 0x01;
	}
	else{
		LOGGER_INFO( "File error");
		// DeleteFile
		req[2] = 0x00;
	}
	rec->sendTcpFrame(req,2,true);
}

/** REC_TO_SRV_endFilesTransfert 
 * Get signal for all files will be send for recording
 **/
void Recorder::REC_TO_SRV_endFilesTransfert( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;

	if ( rec->_IdRecorder == 0 || size == 0 )
		return;

	int idRecording	= 0;
	for (unsigned long  i = 0; i < size ; i++ ) idRecording = ( idRecording << 8) | data[i];
	LOGGER_INFO("All File Transfered from recording: " << idRecording );

	rec->_Tcp->sendAck( ( BYTE * )GET_END_TRANSFERT_FILES);
}

/** REC_TO_SRV_createRecording
 * Receive a signal to create a recording
 **/
void Recorder::REC_TO_SRV_createRecording( BYTE* data, unsigned long size, void *sender) {	
	Recorder *rec = (Recorder*) sender;
	if ( rec->_IdRecorder == 0x00 || rec->_IdUserRecording == 0x00 ){
		LOGGER_WARN("Create Recording request ignore");
		return;
	}
	uint64_t idRecording = Mysql::createRecording ( rec->_IdRecorder , rec->_IdUserRecording );
	if ( idRecording == 0x00 ){
		LOGGER_WARN( "Cannot create a Recording");
		return;
	}

	LOGGER_INFO("Create new Recording " << idRecording << " to Recorder: " << rec->_IdRecorder );

	BYTE req[] = { SEND_RECORDING_ID , 
		(uint8_t) (( idRecording >> 56 ) & 0xFF),
		(uint8_t) (( idRecording >> 48 ) & 0xFF),
		(uint8_t) (( idRecording >> 40 ) & 0xFF),
		(uint8_t) (( idRecording >> 32 ) & 0xFF),
		(uint8_t) (( idRecording >> 24 ) & 0xFF),
		(uint8_t) (( idRecording >> 16 ) & 0xFF),
		(uint8_t) (( idRecording >> 8  ) & 0xFF),
		(uint8_t) (  idRecording	 & 0xFF)
	};
	rec->sendTcpFrame(req,9,true);
}

/** REC_TO_SRV_recordingEnd
 * Get signal for the end of the recording
 **/
void Recorder::REC_TO_SRV_recordingEnd( BYTE* data, unsigned long size, void *sender ){
	if ( size != 8 ){
		cout << "lenght error getRecordingEnd: " << size  << endl;
		return;
	}

	uint64_t idRecording = (uint64_t) ( (uint64_t) data[0] << 56 | (uint64_t) data[1] << 48 | (uint64_t )data[2] << 40 | (uint64_t) data[3] << 32 |
			(uint64_t) data[4] << 24 | (uint64_t) data[5] << 16 | (uint64_t) data[6] << 8  | (uint64_t) data[7]);

	LOGGER_INFO("Recording Finish: " << idRecording );
	// TODO : MYSSQL SET STATE OF RECORD TO FALSE
}
