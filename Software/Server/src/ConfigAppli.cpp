#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "Config.h"
#include "define.h"
#include "ConfigAppli.h"
#include <sys/stat.h>
#include "Mysql.h"
#include "Recorder.h"

#define SIZE_BUFFER 1024

using namespace std;

int ConfigAppli::_Port;
int ConfigAppli::_Socket;
struct sockaddr_in ConfigAppli::_server , ConfigAppli::_client;
pthread_t ConfigAppli::_ThreadListenner;

string getValue ( string req, string id, int* err ){
	int start = req.find( SSTR("<" << id << ">") ) , stop = req.find( SSTR("</" << id << ">") );
	if ( start != -1  && stop != -1 ){
		*err = 1;
		return req.substr( start + 2 + id.length()  , stop - start - 2 - id.length() );
	}
	*err = 0;
	return "";

}


/****************  Constructor  ****************/ 
ConfigAppli::ConfigAppli ( int port  ){
	_Port = port;
}


/****************  Functions  ****************/ 
bool ConfigAppli::createSocket(){

	//Create socket
	_Socket = socket(AF_INET , SOCK_STREAM , 0);
	if ( _Socket == -1)
	{
		LOGGER_ERROR("TCP : Could not create socket");
		return false;
	}

	//Prepare the sockaddr_in structure
	_server.sin_family = AF_INET;
	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_port = htons( _Port );

	//Bind
	if( bind(_Socket,(struct sockaddr *)&_server , sizeof(_server)) < 0)
	{
		//print the error message
		LOGGER_ERROR("Tcp bind failed. Error");
		return false;
	}
	LOGGER_DEBUG("TCP : Bind done");


	if( pthread_create( &_ThreadListenner , NULL ,  &run , NULL ) < 0)
	{
		LOGGER_ERROR("TCP : Error: could not create thread");
	}
	return true;
}

void* ConfigAppli::run( void* d){
	int client_sock;
	//Listen
	listen(_Socket , 3);

	//Accept and incoming connection
	LOGGER_DEBUG("Tcp : Waiting for incoming connections...");
	int c = sizeof(struct sockaddr_in);


	int read_size;
	char client_message[SIZE_BUFFER];

	while( ( client_sock = accept( _Socket, (struct sockaddr *)&_client, (socklen_t*)&c)) )
	{
		LOGGER_DEBUG("Connection accepted from Api Configuration");

		while( (read_size  = recv( client_sock , client_message , SIZE_BUFFER , 0)) > 0 )
		{
			LOGGER_VERB("Admin API : " << client_message);
			decodeRequest( client_message );
		}
		LOGGER_VERB("Out Recv");
	}

	LOGGER_DEBUG("TCP : Close socket");
	return NULL;
}

bool ConfigAppli::sendData( string data ){
	cout << data << endl;
	return true;
}

void ConfigAppli::decodeRequest ( char *req ){
	int ok;
	string type = getValue ( req , "type", &ok );
	if ( ok == 0 ){
		LOGGER_ERROR("TCP Config Format error");
		return;
	}

	/// GET
	if ( type.compare( "need_recorders" ) == 0 )
		getRecorders();

	else if ( type.compare( "need_users_recorders" ) == 0 )
		getUsersRecorders();

	else if ( type.compare( "need_users_website" ) == 0 )
		getUsersWebsite();

	else if ( type.compare( "need_rooms" ) == 0 )
		getRooms();

	else if ( type.compare( "need_cards" ) == 0 )
		getCards();

	/// Create
	else if ( type.compare( "create_rooms" ) == 0 )
		createRooms( req );
		
	else if (  type.compare( "create_userwebsite" ) == 0 )
		createUsersWebSite( req );
		
	else if (  type.compare( "create_cards" ) == 0 )
		createCards ( req );	
	
	else if (  type.compare( "create_recorders" ) == 0 )
		createRecorders ( req );	
		
	
	/// Updates
	else if (  type.compare( "update_rooms" ) == 0 )
		updateRooms ( req );
		
	else if (  type.compare( "update_userswebsite" ) == 0 )
		updateUsersWebSite ( req );
		
	else if (  type.compare( "update_usersrecorders" ) == 0 )
		updateUsersRecorder ( req );
		
	else if (  type.compare( "update_cards" ) == 0 )
		updateCards ( req );
		
	else
		cout << type << endl;
} 




/****************  Getters ****************/
void ConfigAppli::getCards(){
	Result* res = Mysql::getCards();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_CARDS</type>";
	string idCard, iduser, number;

	while ( res->Next() ){
		idCard    = res->GetCurrentRow()->GetField(1);
		iduser    = res->GetCurrentRow()->GetField(1);
		number  = res->GetCurrentRow()->GetField(2);

		sRes = SSTR( sRes << endl << "<id>" << idCard << "</id><iduser>" << iduser << "</roomname><number>" << number << "</number>");
	}
	sendData ( sRes );
}

void ConfigAppli::getRooms(){
	Result* res = Mysql::getRooms();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_ROOMS</type>";
	uint64_t idRoom ;
	string roomName, description;

	while ( res->Next() ){
		idRoom    = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		roomName = res->GetCurrentRow()->GetField(2);
		description = res->GetCurrentRow()->GetField(3);

		sRes = SSTR( 	sRes << endl << "<id>" << idRoom << "</id><roomname>" << roomName << "</roomname><description>" << description << "</description>");
	}
	sendData ( sRes );
}

void ConfigAppli::getRecorders(){
	Result* res = Mysql::getRecorders();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_RECORDERS</type>";

	//bool isRecording;
	string idRec, idRoom, idCModule, idRModule, roomName, mac, status, ip ;
	Recorder *rec;

	while ( res->Next() ){
		idRec		= res->GetCurrentRow()->GetField(1);
		idRoom		= res->GetCurrentRow()->GetField(2);
		roomName	= res->GetCurrentRow()->GetField(3);
		mac		= res->GetCurrentRow()->GetField(4);
		idCModule	= res->GetCurrentRow()->GetField(5);
		idRModule	= res->GetCurrentRow()->GetField(6);

		rec = Recorder::getRecorderByMac( (BYTE*) mac.c_str() );

		status = ( rec != NULL && rec->isTcpConnected() ) ? "CONNECTED" : "UNCONNECTED" ;
		ip = ( rec != NULL ) ? rec->_IpAddr : "" ;

		sRes = SSTR ( 	sRes << endl << "<id>" << idRec  << "</id><status>" << status << "</status><mac>" 
				<< mac << "</mac><ip>" << ip << "</ip><idCModule>" << idCModule << "</idCModule><idRModule>" << idRModule << "</idRModule><roomid>" 
				<< idRoom << "</roomid><roomname>" << roomName << "</roomname>"
			    );

		if ( status.compare("CONNECTED") == 0 ){
			sRes = SSTR ( sRes << "<filesinqueue>" << rec->filesInWait << "</filesinqueue><isRecording>" << rec->isRecording() << "</isRecording>" );
		} 
	}

	for ( list<UnconnectedClient*>::iterator it = Recorder::_UnconnectedClients.begin() ; it != Recorder::_UnconnectedClients.end(); ++it){
		UnconnectedClient * uc = *it;
		sRes = SSTR ( sRes << endl << "<status>UNASSOCIATED</status><mac>" << uc->_MacAddress << "</mac>" );
	}
	sendData( sRes );
}

void ConfigAppli::getUsersWebsite(){
	Result* res = Mysql::getUsers();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_USERS_WEBSITE</type>";

	uint64_t idUser ;
	string firstName, lastName, email;
	string dateRegistration;

	while ( res->Next() ){
		idUser    = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		firstName = res->GetCurrentRow()->GetField(2);
		lastName = res->GetCurrentRow()->GetField(3);
		dateRegistration = res->GetCurrentRow()->GetField(4);

		sRes = SSTR( sRes << endl << "<id>" << idUser << "</id><firstname>" << firstName << "</firstname><lastname>" << lastName << "</lastname><email>" << email << "</email><dateregistration>"
				<< dateRegistration << "</dateregistration>"
			   );
	}
	sendData ( sRes );
}

void ConfigAppli::getUsersRecorders(){
	Result* res = Mysql::getUsersRecorders();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_USERS_RECORDERS</type>";
	uint64_t idUser ;
	string firstName, lastName, email;
	string dateBegin, dateEnd;

	while ( res->Next() ){
		idUser    = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		firstName = res->GetCurrentRow()->GetField(2);
		lastName  = res->GetCurrentRow()->GetField(3);
		email 	  = res->GetCurrentRow()->GetField(4);
		dateBegin = res->GetCurrentRow()->GetField(5);
		dateEnd   = res->GetCurrentRow()->GetField(6);

		sRes = SSTR( 	sRes << endl << "<id>" << idUser << "</id><firstname>" << firstName << "</firstname><lastname>" << lastName << "</lastname><email>" << email << "</email><datebegin>"
				<< dateBegin << "</datebegin><dateend>" << dateEnd << "</dateend>"
			   );
	}
	sendData ( sRes );
}


/****************  Creates ****************/
void ConfigAppli::createRooms( string req ){
	string sRes = "<type>CREATE_ROOMS</type>";
	istringstream f( req );
	string tmp, roomName, roomDescription;
	uint64_t idRoom;
	int ok = 0;
	while ( getline(f,tmp) ){
		roomName = getValue(tmp,(string)"name", &ok );
		if ( ok == 0 ) continue;
		roomDescription = getValue(tmp,(string) "description", &ok);

		idRoom = Mysql::createRoom ( roomName, roomDescription );
		sRes = SSTR ( sRes << endl << "<idRoom>" << idRoom << "</idRoom><roomname>" << roomName << "</roomname>");
	}
	sendData ( sRes );
}

void ConfigAppli::createRecorders( string req ){
	string sRes = "<type>CREATE_RECORDERS</type>";
	istringstream f( req );
	string tmp, sidRoom, addressMac;
	uint64_t idRoom, idRecorder;
	int ok = 0;
	
	while ( getline(f,tmp) ){
		addressMac = getValue(tmp,(string)"mac", &ok );
		if ( ok == 0 ) continue;
		sidRoom = getValue(tmp,(string)"idroom", &ok );
		idRoom = strtoull( sidRoom.c_str(), NULL, 0 );
		if ( ok == 0 || idRoom == 0 ) continue;

		idRecorder = Mysql::createRecorder( idRoom, addressMac );
		sRes = SSTR ( sRes << endl << "<idrecorder>" << idRecorder << "</idrecorder><mac>" << addressMac << "</mac>");
	}
	sendData ( sRes );
}

void ConfigAppli::createUsersWebSite( string req ){
	string sRes = "<type>CREATE_USERSWEBSITE</type>";
	istringstream f( req );
	string tmp, firstName, lastName, password, email, DateRegistration;
	uint64_t idUser = 0;
	int ok = 0;
	
	while ( getline(f,tmp) ){
		firstName = getValue(tmp,(string)"fistname", &ok );
		if ( ok == 0 ) continue;
		
		lastName = getValue(tmp,(string)"lastname", &ok );
		if ( ok == 0 ) continue;
		
		password = getValue(tmp,(string)"password", &ok );
		if ( ok == 0 ) continue;
		
		email = getValue(tmp,(string)"email", &ok );
		if ( ok == 0 ) continue;
		
		DateRegistration = getValue(tmp,(string)"dateregistration", &ok );
		if ( ok == 0 ) continue;

		idUser = Mysql::createUserWebSite ( firstName, lastName, password, email , DateRegistration );
		sRes = SSTR ( sRes << endl << "<iduser>" << idUser << "</iduser><email>" << email << "</email>");
	}
	sendData ( sRes );
}

void ConfigAppli::createUsersRecorder( string req ){
	string sRes = "<type>CREATE_USERSRECORDER</type>";
	istringstream f( req );
	string tmp, firstName, lastName, password, email, dateBegin, dateEnd;
	int ok = 0;
	uint64_t idUser = 0;
	
	while ( getline(f,tmp) ){
		firstName = getValue(tmp,(string)"fistname", &ok );
		if ( ok == 0 ) continue;
		
		lastName = getValue(tmp,(string)"lastname", &ok );
		if ( ok == 0 ) continue;
		
		password = getValue(tmp,(string)"password", &ok );
		if ( ok == 0 ) continue;
		
		email = getValue(tmp,(string)"email", &ok );
		if ( ok == 0 ) continue;
		
		dateBegin = getValue(tmp,(string)"datebegin", &ok );
		if ( ok == 0 ) continue;
		
		dateEnd = getValue(tmp,(string)"dateend", &ok );
		if ( ok == 0 ) continue;

		idUser = Mysql::createUserRecorder ( firstName, lastName, password, email , dateBegin, dateEnd );
		sRes = SSTR ( sRes << endl << "<iduser>" << idUser << "</iduser><email>" << email << "</email>");
	}
	sendData ( sRes );
}

void ConfigAppli::createCards( string req ){
	string sRes = "<type>CREATE_CARDS</type>";
	istringstream f( req );
	string tmp, sidUser, snumber;
	int ok = 0;
	uint64_t idUser = 0, idCard, number;
	
	while ( getline(f,tmp) ){
		snumber = getValue(tmp,(string)"number", &ok );
		if ( ok == 0 ) continue;
		
		sidUser = getValue(tmp,(string)"iduser", &ok );
		
		idUser = strtoull( sidUser.c_str(), NULL, 0);
		number = strtoull( snumber.c_str(), NULL, 0);

		idCard = Mysql::createCard ( number , idUser );
		sRes = SSTR ( sRes << endl << "<idcard>" << (( idCard == 0x00 ) ? -1: idCard) << "</idcard><number>" << snumber << "</number>");
	}
	sendData ( sRes );
}


/****************  Updates ****************/
void ConfigAppli::updateRooms( string req ){
	string sRes = "<type>UPDATES_ROOMS</type>";
	istringstream f( req );
	string tmp, roomName, roomDescription, id;
	bool verif;
	uint64_t idRoom;
	int ok = 0, Ename = 0, Edescription = 0;
	while ( getline(f,tmp) ){
		id = getValue(tmp,(string)"id", &ok );
		idRoom = strtoull( id.c_str() , NULL, 0 );
		if ( ok == 0 || idRoom == 0 ) continue;
		
		roomName = getValue(tmp,(string)"name", &Ename );
		if ( ok == 0 ) continue;
		roomDescription = getValue(tmp,(string) "description", &Edescription);

		verif = Mysql::updateRoom ( idRoom , (Ename == 1), roomName, ( Edescription == 1), roomDescription );
		sRes = SSTR ( sRes << endl << "<idRoom>" << idRoom << "</idRoom><succes>" << verif << "</succes>");
	}
	sendData ( sRes );
}

void ConfigAppli::updateUsersWebSite( string req ){
	string sRes = "<type>UPDATES_USERSWEBSITE</type>";
	istringstream f( req );
	string tmp, id, fName, lName, pwd, email;
	bool verif;
	uint64_t idUser;
	int ok = 0, eFName = 0, eLName = 0, ePwd = 0, eMail = 0;
	
	while ( getline(f,tmp) ){
		id = getValue(tmp,(string)"id", &ok );
		idUser = strtoull( id.c_str() , NULL, 0 );
		if ( ok == 0 || idUser == 0 ) continue;
		
		fName = getValue(tmp,(string)"firstname", &eFName );
		lName = getValue(tmp,(string)"lastname", &eLName );
		pwd   = getValue(tmp,(string)"password", &ePwd   );
		email = getValue(tmp,(string)"email", &eMail  );

		verif = Mysql::updateUserTable ( idUser, ( eFName == 1 ), fName, ( eLName == 1) , lName, ( ePwd == 1 ), pwd, ( eMail == 1) , email );
		sRes = SSTR ( sRes << endl << "<iduser>" << idUser << "</iduser><succes>" << verif << "</succes>");
	}
	sendData ( sRes );
}

void ConfigAppli::updateUsersRecorder( string req ){
	string sRes = "<type>UPDATES_USERSWEBSITE</type>";
	istringstream f( req );
	string tmp, id, fName, lName, pwd, email ,dateBegin, dateEnd;
	bool verif;
	uint64_t idUser;
	int ok = 0, eFName = 0, eLName = 0, ePwd = 0, eMail = 0, eBegin = 0, eEnd = 0;
	
	while ( getline(f,tmp) ){
		id = getValue(tmp,(string)"id", &ok );
		idUser = strtoull( id.c_str() , NULL, 0 );
		if ( ok == 0 || idUser == 0 ) continue;
		
		fName = getValue(tmp,(string)"firstname", &eFName );
		lName = getValue(tmp,(string)"lastname", &eLName );
		pwd   = getValue(tmp,(string)"password", &ePwd   );
		email = getValue(tmp,(string)"email", &eMail  );
		dateBegin = getValue(tmp,(string)"email", &eBegin  );
		dateEnd = getValue(tmp,(string)"email", &eEnd  );

		verif  = Mysql::updateUserTable ( idUser, ( eFName == 1 ), fName, ( eLName == 1) , lName, ( ePwd == 1 ), pwd, ( eMail == 1) , email );
		verif |= Mysql::updateUserRecorderTable ( idUser, ( eBegin == 1) , dateBegin, ( eEnd == 1) , dateEnd );
		
		sRes = SSTR ( sRes << endl << "<iduser>" << idUser << "</iduser><succes>" << verif << "</succes>");
	}
	sendData ( sRes );
}

void ConfigAppli::updateCards( string req ){
	string sRes = "<type>UPDATE_CARDS</type>";
	istringstream f( req );
	string tmp, sidCard, sidUser;
	int ok = 0;
	uint64_t idUser = 0, idCard;
	bool verif;
	
	while ( getline(f,tmp) ){
		sidCard = getValue(tmp,(string)"idcard", &ok );
		if ( ok == 0 ) continue;
		
		sidUser = getValue(tmp,(string)"iduser", &ok );
		if ( ok == 0 ) continue; 
		
		idUser = strtoull( sidUser.c_str(), NULL, 0);
		idCard = strtoull( sidCard.c_str(), NULL, 0);

		verif = Mysql::updateCard ( idCard , idUser );
		sRes = SSTR ( sRes << endl << "<idcard>" << idCard << "</idcard><success>" << verif << "</success>");
	}
	sendData ( sRes );
}

void ConfigAppli::updateRecorders( string req ){
	string sRes = "<type>UPDATE_RECORDERS</type>";
	istringstream f( req );
	string tmp, sidRoom, sIdRecorder;
	uint64_t idRoom, idRecorder;
	int ok = 0;
	bool succes;
	
	while ( getline(f,tmp) ){
		sIdRecorder = getValue(tmp,(string)"idrecorder", &ok );
		idRecorder = strtoull ( sIdRecorder.c_str(), NULL, 0);
		if ( ok == 0 || idRecorder == 0) continue;
		
		sidRoom = getValue(tmp,(string)"idroom", &ok );
		idRoom = strtoull( sidRoom.c_str(), NULL, 0 );
		if ( ok == 0 || idRoom == 0 ) continue;
		
		succes = Mysql::updateRecorder ( idRecorder , idRoom );
		sRes = SSTR ( sRes << endl << "<idrecorder>" << idRecorder << "</idrecorder><success>" << succes << "</success>");
	}
	sendData ( sRes );
}

/****************  Deletes ****************/
