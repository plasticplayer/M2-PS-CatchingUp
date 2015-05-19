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


inline string prepareString ( string req ){
	// TODO : Remove Special Caracters
	return req;
}

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
	
	else if (  type.compare( "create_userrecorder" ) == 0 )
		createUsersRecorder( req );
		
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

	string sRes = "<type>GET_CARDS</type><cards>";
	string idCard, iduser, number;

	while ( res->Next() ){
		idCard    = res->GetCurrentRow()->GetField(1);
		iduser    = res->GetCurrentRow()->GetField(1);
		number    = res->GetCurrentRow()->GetField(2);

		sRes = SSTR( sRes << endl << "<card> <id>" << idCard << "</id><iduser>" << iduser << "</roomname><number>" << number << "</number> </card>");
	}
	sendData ( SSTR(sRes << "</cards>") );
}

void ConfigAppli::getRooms(){
	Result* res = Mysql::getRooms();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_ROOMS</type><rooms>";
	uint64_t idRoom ;
	string roomName, description;

	while ( res->Next() ){
		idRoom   	= strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		roomName  	= res->GetCurrentRow()->GetField(2);
		description = res->GetCurrentRow()->GetField(3);

		sRes = SSTR( 	sRes << endl << "<room><id>" << idRoom << "</id><roomname>" << roomName << "</roomname><description>" << description << "</description></room>");
	}
	sendData ( SSTR( sRes << "</rooms>") );
}

void ConfigAppli::getRecorders(){
	Result* res = Mysql::getRecorders();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_RECORDERS</type><recorders>";

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


		sRes = SSTR ( 	sRes << endl << "<recorder><id>" << idRec  << "</id><status>" << status << "</status><mac>" 
				<< mac << "</mac><ip>" << ip << "</ip><idCModule>" << idCModule << "</idCModule><idRModule>" << idRModule << "</idRModule><roomid>" 
				<< idRoom << "</roomid><roomname>" << roomName << "</roomname>" );
		
		 
		if ( status.compare("CONNECTED") == 0 )
			sRes = SSTR( sRes <<"<filesinqueue>" << rec->filesInWait << "</filesinqueue><isRecording>" << rec->isRecording() << "</isRecording></recorder>");
		else
			sRes = SSTR ( sRes << "</recorder>" );
	}

	for ( list<UnconnectedClient*>::iterator it = Recorder::_UnconnectedClients.begin() ; it != Recorder::_UnconnectedClients.end(); ++it){
		UnconnectedClient * uc = *it;
		sRes = SSTR ( sRes << endl << "<recorder><status>UNASSOCIATED</status><mac>" << uc->_MacAddress << "</mac></recorder>" );
	}
	sendData( SSTR( sRes << "</recorders>") );
}

void ConfigAppli::getUsersWebsite(){
	Result* res = Mysql::getUsers();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_USERS_WEBSITE</type><userswebsite>";

	uint64_t idUser ;
	string firstName, lastName, email;
	string dateRegistration;

	while ( res->Next() ){
		idUser    = strtoull ( res->GetCurrentRow()->GetField(1),NULL,0);
		firstName = res->GetCurrentRow()->GetField(2);
		lastName = res->GetCurrentRow()->GetField(3);
		dateRegistration = res->GetCurrentRow()->GetField(4);

		sRes = SSTR( sRes << endl << "<user><id>" << idUser << "</id><firstname>" << firstName << "</firstname><lastname>" << lastName << "</lastname><email>" << email << "</email><dateregistration>"
				<< dateRegistration << "</dateregistration></user>"
			   );
	}
	sendData ( SSTR( sRes << "</userswebsite>") );
}

void ConfigAppli::getUsersRecorders(){
	Result* res = Mysql::getUsersRecorders();
	if ( res == NULL ) 
		return;

	string sRes = "<type>GET_USERS_RECORDERS</type><usersrecorder>";
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

		sRes = SSTR( 	sRes << endl << "<user><id>" << idUser << "</id><firstname>" << firstName << "</firstname><lastname>" << lastName << "</lastname><email>" << email << "</email><datebegin>"
				<< dateBegin << "</datebegin><dateend>" << dateEnd << "</dateend></user>"
			   );
	}
	sendData ( SSTR( sRes << "</usersrecorder>"));
}



/****************  Creates ****************/
void ConfigAppli::createRooms( string req ){
	string sRes = "<type>CREATE_ROOMS</type><rooms>";
	istringstream f( req );
	string tmp, roomName, roomDescription, line;
	uint64_t idRoom;
	int ok = 0;
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string) "room", &ok);
		if ( ok == 0 )
			continue;
			
		roomName = getValue(line,(string)"name", &ok );
		if ( ok == 0 ) continue;
		roomDescription = getValue(line,(string) "description", &ok);
	
		roomName = prepareString(roomName);
		roomDescription = prepareString(roomDescription);
		
		idRoom = Mysql::createRoom ( roomName, roomDescription );
		sRes = SSTR ( sRes << endl << "<room><idRoom>" << idRoom << "</idRoom><roomname>" << roomName << "</roomname></room>");
	}
	sendData ( SSTR( sRes << "</rooms>" ));
}

void ConfigAppli::createRecorders( string req ){
	string sRes = "<type>CREATE_RECORDERS</type><recorders>";
	istringstream f( req );
	string tmp, sidRoom, addressMac;
	uint64_t idRoom, idRecorder;
	int ok = 0;
	
	while ( getline(f,tmp) ){
		addressMac = prepareString(getValue(tmp,(string)"mac", &ok ));
		if ( ok == 0 ) continue;
		
		sidRoom = getValue(tmp,(string)"idroom", &ok );
		idRoom = strtoull( sidRoom.c_str(), NULL, 0 );
		if ( ok == 0 || idRoom == 0 ) continue;

		idRecorder = Mysql::createRecorder( idRoom, addressMac );
		sRes = SSTR ( sRes << endl << "<recorder><idrecorder>" << idRecorder << "</idrecorder><mac>" << addressMac << "</mac></recorder>");
	}
	sendData ( SSTR( sRes << "</recorders>") );
}

void ConfigAppli::createUsersWebSite( string req ){
	string sRes = "<type>CREATE_USERSWEBSITE</type><userswebsite>";
	istringstream f( req );
	string tmp, firstName, lastName, password, email, DateRegistration, line;
	uint64_t idUser = 0;
	int ok = 0;
	
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string) "user",&ok);
		if ( ok == 0 )
			continue;
			
		firstName = prepareString(getValue(line,(string)"fistname", &ok ));
		if ( ok == 0 ) continue;
		
		lastName = prepareString(getValue(line,(string)"lastname", &ok ));
		if ( ok == 0 ) continue;
		
		password = prepareString(getValue(line,(string)"password", &ok ));
		if ( ok == 0 ) continue;
		
		email = prepareString(getValue(line,(string)"email", &ok ));
		if ( ok == 0 ) continue;
		
		DateRegistration = getValue(line,(string)"dateregistration", &ok );
		if ( ok == 0 ) continue;

		idUser = Mysql::createUserWebSite ( firstName, lastName, password, email , DateRegistration );
		sRes = SSTR ( sRes << endl << "<user><iduser>" << idUser << "</iduser><email>" << email << "</email></user>");
	}
	sendData ( SSTR( sRes << "</userswebsite>") );
}

void ConfigAppli::createUsersRecorder( string req ){
	string sRes = "<type>CREATE_USERSRECORDER</type><usersrecorder>";
	istringstream f( req );
	string tmp, firstName, lastName, password, email, dateBegin, dateEnd, line;
	int ok = 0;
	uint64_t idUser = 0;
	
	while ( getline(f,tmp) ){
		line = getValue(tmp, (string) "user", &ok );
		if ( ok == 0 )
			continue;
			
		firstName = prepareString(getValue(line,(string)"fistname", &ok ));
		if ( ok == 0 ) continue;
		
		lastName = prepareString(getValue(line,(string)"lastname", &ok ));
		if ( ok == 0 ) continue;
		
		password = prepareString(getValue(line,(string)"password", &ok ));
		if ( ok == 0 ) continue;
		
		email = prepareString(getValue(line,(string)"email", &ok ));
		if ( ok == 0 ) continue;
		
		dateBegin = getValue(line,(string)"datebegin", &ok );
		if ( ok == 0 ) continue;
		
		dateEnd = getValue(line,(string)"dateend", &ok );
		if ( ok == 0 ) continue;

		
		idUser = Mysql::createUserRecorder ( firstName, lastName, password, email , dateBegin, dateEnd );
		sRes = SSTR ( sRes << endl << "<user><iduser>" << idUser << "</iduser><email>" << email << "</email></user>");
	}

	sendData ( SSTR(sRes << "</usersrecorder>" ));
}

void ConfigAppli::createCards( string req ){
	string sRes = "<type>CREATE_CARDS</type><cards>";
	istringstream f( req );
	string tmp, sidUser, snumber, line;
	int ok = 0;
	uint64_t idUser = 0, idCard, number;
	
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string) "card",&ok);
		if ( ok == 0 ) continue;
		
		snumber = prepareString(getValue(line,(string)"number", &ok ));
		if ( ok == 0 ) continue;
		
		sidUser = getValue(line,(string)"iduser", &ok );
		
		idUser = strtoull( sidUser.c_str(), NULL, 0);
		number = strtoull( snumber.c_str(), NULL, 0);

		idCard = Mysql::createCard ( number , idUser );
		sRes = SSTR ( sRes << endl << "<card><idcard>" << (( idCard == 0x00 ) ? -1: idCard) << "</idcard><number>" << snumber << "</number></card>");
	}
	sendData ( SSTR( sRes << "</cards>") );
}



/****************  Updates ****************/
void ConfigAppli::updateRooms( string req ){
	string sRes = "<type>UPDATES_ROOMS</type><rooms>";
	istringstream f( req );
	string tmp, roomName, roomDescription, id, line;
	bool verif;
	uint64_t idRoom;
	int ok = 0, Ename = 0, Edescription = 0;
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string) "room", &ok);
		if ( ok == 0 ) continue;
		
		id = getValue(line,(string)"id", &ok );
		idRoom = strtoull( id.c_str() , NULL, 0 );
		if ( ok == 0 || idRoom == 0 ) continue;
		
		roomName = prepareString(getValue(line,(string)"name", &Ename ));
		
		roomDescription = prepareString(getValue(line,(string) "description", &Edescription));

		verif = Mysql::updateRoom ( idRoom , (Ename == 1), roomName, ( Edescription == 1), roomDescription );
		sRes = SSTR ( sRes << endl << "<room><idRoom>" << idRoom << "</idRoom><succes>" << verif << "</succes></room>");
	}
	sendData ( SSTR( sRes << "</rooms>" ));
}

void ConfigAppli::updateUsersWebSite( string req ){
	string sRes = "<type>UPDATES_USERSWEBSITE</type><userswebsite>";
	istringstream f( req );
	string tmp, id, fName, lName, pwd, email, line;
	bool verif;
	uint64_t idUser;
	int ok = 0, eFName = 0, eLName = 0, ePwd = 0, eMail = 0;
	
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string ) "user", &ok);
		if ( ok == 0 )
			continue;
			
		id = getValue(line,(string)"id", &ok );
		idUser = strtoull( id.c_str() , NULL, 0 );
		if ( ok == 0 || idUser == 0 ) continue;
		
		fName = prepareString(getValue(line,(string)"firstname", &eFName ));
		lName = prepareString(getValue(line,(string)"lastname" , &eLName ));
		pwd   = prepareString(getValue(line,(string)"password" , &ePwd   ));
		email = prepareString(getValue(line,(string)"email"    , &eMail  ));

		verif = Mysql::updateUserTable ( idUser, ( eFName == 1 ), fName, ( eLName == 1) , lName, ( ePwd == 1 ), pwd, ( eMail == 1) , email );
		sRes = SSTR ( sRes << endl << "<user><iduser>" << idUser << "</iduser><succes>" << verif << "</succes></user>");
	}
	sendData ( SSTR ( sRes << "</userswebsite>") );
}

void ConfigAppli::updateUsersRecorder( string req ){
	string sRes = "<type>UPDATES_USERSWEBSITE</type><usersrecorder>";
	istringstream f( req );
	string tmp, id, fName, lName, pwd, email ,dateBegin, dateEnd, line;
	bool verif;
	uint64_t idUser;
	int ok = 0, eFName = 0, eLName = 0, ePwd = 0, eMail = 0, eBegin = 0, eEnd = 0;
	
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string) "user", &ok);
		if ( ok == 0 ) continue;
		
		id = getValue(line,(string)"id", &ok );
		idUser = strtoull( id.c_str() , NULL, 0 );
		if ( ok == 0 || idUser == 0 ) continue;
		
		fName = prepareString(getValue(line,(string)"firstname", &eFName ));
		lName = prepareString(getValue(line,(string)"lastname", &eLName ));
		pwd   = prepareString(getValue(line,(string)"password", &ePwd   ));
		email = prepareString(getValue(line,(string)"email", &eMail  ));
		dateBegin = getValue(line,(string)"email", &eBegin  );
		dateEnd = getValue(line,(string)"email", &eEnd  );

		verif  = Mysql::updateUserTable ( idUser, ( eFName == 1 ), fName, ( eLName == 1) , lName, ( ePwd == 1 ), pwd, ( eMail == 1) , email );
		verif |= Mysql::updateUserRecorderTable ( idUser, ( eBegin == 1) , dateBegin, ( eEnd == 1) , dateEnd );
		
		sRes = SSTR ( sRes << endl << "<user><iduser>" << idUser << "</iduser><succes>" << verif << "</succes></user>");
	}
	sendData ( SSTR(sRes << "</usersrecorder>") );
}

void ConfigAppli::updateCards( string req ){
	string sRes = "<type>UPDATE_CARDS</type><cards>";
	istringstream f( req );
	string tmp, sidCard, sidUser, line;
	int ok = 0;
	uint64_t idUser = 0, idCard;
	bool verif;
	
	while ( getline(f,tmp) ){
		line = getValue ( tmp , (string) "card", &ok);
		if ( ok == 0) continue;
		
		sidCard = getValue(line,(string)"idcard", &ok );
		if ( ok == 0 ) continue;
		
		sidUser = getValue(line,(string)"iduser", &ok );
		if ( ok == 0 ) continue; 
		
		idUser = strtoull( sidUser.c_str(), NULL, 0);
		idCard = strtoull( sidCard.c_str(), NULL, 0);

		verif = Mysql::updateCard ( idCard , idUser );
		sRes = SSTR ( sRes << endl << "<card><idcard>" << idCard << "</idcard><success>" << verif << "</success></card>");
	}
	sendData ( SSTR (sRes << "</cards>") );
}

void ConfigAppli::updateRecorders( string req ){
	string sRes = "<type>UPDATE_RECORDERS</type><recorders>";
	istringstream f( req );
	string tmp, sidRoom, sIdRecorder, line;
	uint64_t idRoom, idRecorder;
	int ok = 0;
	bool succes;
	
	while ( getline(f,tmp) ){
		line = getValue( tmp, (string) "recorder",&ok );
		if ( ok == 0 )
			continue;
			
		sIdRecorder = getValue(tmp,(string)"idrecorder", &ok );
		idRecorder = strtoull ( sIdRecorder.c_str(), NULL, 0);
		if ( ok == 0 || idRecorder == 0) continue;
		
		sidRoom = getValue(tmp,(string)"idroom", &ok );
		idRoom = strtoull( sidRoom.c_str(), NULL, 0 );
		if ( ok == 0 || idRoom == 0 ) continue;
		
		succes = Mysql::updateRecorder ( idRecorder , idRoom );
		sRes = SSTR ( sRes << endl << "<recorder><idrecorder>" << idRecorder << "</idrecorder><success>" << succes << "</success></recorder>");
	}
	sendData ( SSTR( sRes << "</recorders>" ));
}


/****************  Deletes ****************/
