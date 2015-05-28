#define IMAGE_SIZE_MAX 2000
#define PASS_SIZE 10
#include <Ftp.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "define.h"
#include "Recorder.h"
#include "Communication.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "Mysql.h"

list<Recorder *> Recorder::_Recorders;
list<UnconnectedClient*> Recorder::_UnconnectedClients;
using namespace std;

void gen_random(char *s, const int len) {
    static const char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len-1; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len -1] = '\0';
}
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
	this->filesInWait = 0;
	this->_IsRecording = false;
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


void Recorder::decoTcp( void* recorder ){
	Recorder *rec = ( Recorder*) recorder;
	cout << "Tcp close: " << rec->_IpAddr << endl;
	rec->_Tcp = NULL;
	//this->_Tcp = NULL;
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

Recorder* Recorder::getRecorderById( uint64_t id ){
	if ( _Recorders.empty() )
		return NULL;

	for ( list<Recorder*>::iterator it = _Recorders.begin(); it != _Recorders.end(); ++it ){
		Recorder *rec = *it;
		if ( rec->_IdRecorder == id )
			return rec;
	}
	return NULL;
}

Recorder* Recorder::getRecorderByMac ( BYTE* mac ){
	for ( list<Recorder*>::iterator it = _Recorders.begin(); it != _Recorders.end(); ++it ){
		Recorder *rec = *it;
		if ( memcmp(mac,rec->_MacAddress, 12) == 0 )
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
}

void Recorder::setUdpSocket ( void* sock, int size ){
	if ( this->_UdpSocket != NULL )
		free ( this->_UdpSocket );

	this->_UdpSocket = malloc( size );
	memcpy( this->_UdpSocket , sock, size );

}

bool Recorder::Parring(){
	_stateParring = WORKING;
	SRV_TO_REC_sendParring();
	while ( _stateParring == WORKING ) usleep(25);
	return (_stateParring == PASS);
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

bool Recorder::getImage ( char* image, int* size ){
	if ( isTcpConnected() == false )
		return false;

	_stateGetImage = WORKING;
	SRV_TO_REC_askVisuImage( 0x01 );
	while ( _stateGetImage == WORKING ) usleep(10000);

	return ( _stateGetImage == PASS );
}

	
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


void Recorder::addUnconnectedClient ( BYTE* mac ){
	UnconnectedClient *uc;
	for ( list<UnconnectedClient*>::iterator it = Recorder::_UnconnectedClients.begin() ; it != Recorder::_UnconnectedClients.end(); ++it){
		 uc = *it;
		if ( memcmp(uc->_MacAddress, mac, sizeof(uc->_MacAddress) ) == 0 ){
			return;
		}
	}
	uc = ( UnconnectedClient* ) malloc ( sizeof( UnconnectedClient));
	sprintf( (char* )uc->_MacAddress, (char *) mac );
	Recorder::_UnconnectedClients.push_back(uc);
}
void Recorder::delUnconnectedClient ( BYTE* mac ){
	UnconnectedClient *uc;
	for ( list<UnconnectedClient*>::iterator it = Recorder::_UnconnectedClients.begin() ; it != Recorder::_UnconnectedClients.end(); ++it){
		uc = *it;
		if ( memcmp(uc->_MacAddress, mac, sizeof( uc->_MacAddress) ) == 0 ){
			Recorder::_UnconnectedClients.remove(uc);
			free ( uc );
			return;
		}
	}
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
		addUnconnectedClient ( rec->_MacAddress );
		return;
	}
	delUnconnectedClient ( rec->_MacAddress );
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
			(uint8_t) (  rec->_IdUserRecording         & 0xFF)
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
		rec->_stateParring = PASS;
	}
	else{
		LOGGER_ERROR("Parraing FAILED");
		rec->_stateParring = FAILED;
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

	rec->filesInWait = 0;
	for ( unsigned long  i = 0; i < size; i++ ) rec->filesInWait = ( rec->filesInWait << 8) | data[i];
	LOGGER_DEBUG( "Storage Ask from: " << rec->_IdRecorder << " Files in wait: " << rec->filesInWait );
	
	// TODO : Verifiy if can send File
	bool canSend = true;
	if ( canSend ){
		LOGGER_DEBUG("Can Send File");
		char pass[PASS_SIZE+1];
		gen_random(pass, PASS_SIZE);	
		string s = string ( pass );
		//cout << "Rand Password: " << pass  << "::" << s << endl;
		Ftp::_Ftp->deleteUser( SSTR ( rec->_MacAddress ).substr(0,12) );
		Ftp::_Ftp->addUser ( SSTR ( rec->_MacAddress).substr(0,12), s ,CurrentApplicationConfig.FolderPathTmp , false ); 
		BYTE req[2+PASS_SIZE]; 
		req[0] = ANS_STORAGE;
		req[1] = 0x01;
		memcpy(req+2,pass,PASS_SIZE);
		rec->sendTcpFrame(req,PASS_SIZE + 2,true);
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
	if ( size == 0 || size < (unsigned long) ( 9 + data[0] ) )
		return;

	Recorder *rec = (Recorder*) sender;
	if ( rec->_IdRecorder == 0 )
		return;
	
	Ftp::_Ftp->deleteUser( SSTR ( rec->_MacAddress ).substr(0,12) );
	BYTE req[2]; 
	req[0] = ANS_END_FILE_TRANSFERT;
	req[1] = 0x00;
	string s = "", md5 = "";
	unsigned long i ;
	for ( i = 2; i <= data[0] ; i++){
		s = SSTR( s << data[i] );
	}
		
	for (  ; i < size ; i++){
		md5 = SSTR( md5 << data[i] );
	}

	LOGGER_VERB("File transfert : " << s << " MD5: " << md5 ); 
	
	hashwrapper *myWrapper = new md5wrapper();
	try
	{
		myWrapper->test();
	}
	catch(hlException &e)
	{	
		rec->sendTcpFrame(req,2,true);
		return;
	}

	
	string oldPath = SSTR ( CurrentApplicationConfig.FolderPathTmp << s);
	string hash = "";
	uint64_t idRecording = strtoull ( s.substr(0,s.find('/')).c_str(),NULL, 0);
	
	if ( idRecording == 0 ){
		LOGGER_WARN("IdRecording error: " << s.substr(0,s.find('/')));
		rec->sendTcpFrame(req,2,true);
		remove ( ( const char * ) oldPath.c_str() );
		return;
	}
	try
	{
		hash = myWrapper->getHashFromFile((char*)oldPath.c_str() );
	}
	catch(hlException &e)
	{
		rec->sendTcpFrame(req,2,true);
		remove ( ( const char * ) oldPath.c_str() );
		return;
	}

	bool sameChkSum = ( hash.compare( md5 ) == 0);
	LOGGER_DEBUG("Ftp: Compare Checksum: " << sameChkSum );

	delete myWrapper;
	myWrapper = NULL;


	if ( sameChkSum ){
		string newPath =  SSTR( CurrentApplicationConfig.FolderPathMedia << s );
		mkdir((const char *) newPath.substr(0,newPath.find_last_of('/')).c_str(),0777);
 
		string type = newPath.substr( newPath.find_last_of('.'));
		if ( type.compare(".jpeg") == 0 )
			type = "PHOTO_BOARD";
		else if ( type.compare(".h264") == 0 )
			type = "VIDEO_TRACKING";
		else if ( type.compare(".mp3") == 0 )
			type = "SOUND";
		else {
			LOGGER_WARN ( "FILE TYPE ERROR (" << type  << ") DELETE IT.");
			remove ( ( const char * ) oldPath.c_str() );
			req[1] = 0;
			rec->sendTcpFrame(req,2,true);
			return;
		}
		if ( rename ( oldPath.c_str()  , newPath.c_str() )==0){	
			Mysql::addFileToRecording ( idRecording , newPath, type );
		}
		else{
			LOGGER_ERROR( "Error moving file: " << errno << " " << strerror(errno));
			cout << "Last: " << oldPath << "; New: " << newPath << endl;
			rec->sendTcpFrame(req,2,true);
			// TODO : DO Something ...
			return;
		}
		rec->filesInWait--;
		req[1] = 0x01;
	}
	else{
		LOGGER_INFO( "File error");
		remove ( ( const char * ) oldPath.c_str() );
		req[1] = 0x00;
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
	rec->_IsRecording = true;
}


/** REC_TO_SRV_recordingEnd
 * Get signal for the end of the recording
 **/
void Recorder::REC_TO_SRV_recordingEnd( BYTE* data, unsigned long size, void *sender ){
	Recorder * rec = ( Recorder*) sender;
	if ( size != 8 ){
		cout << "lenght error getRecordingEnd: " << size  << endl;
		return;
	}

	uint64_t idRecording = (uint64_t) ( (uint64_t) data[0] << 56 | (uint64_t) data[1] << 48 | (uint64_t )data[2] << 40 | (uint64_t) data[3] << 32 |
			(uint64_t) data[4] << 24 | (uint64_t) data[5] << 16 | (uint64_t) data[6] << 8  | (uint64_t) data[7]);
	if ( idRecording == 0 )
		return;
	rec->_IsRecording = false;
	LOGGER_INFO("Recording Finish: " << idRecording );
	Mysql::stopRecording( idRecording );
}

bool Recorder::isRecording ( ){
	return _IsRecording;
}
