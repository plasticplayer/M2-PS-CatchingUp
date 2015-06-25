//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Config.h"
#include "Ftp.h"
#include "define.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define BUFFSIZE 2000


using namespace std;
Ftp* Ftp::_Ftp = NULL;
//CFtpServer Ftp::_FtpServer;

Ftp::Ftp( int port) {
	_Ftp = this;
	_Port = port;

	_FtpServer.SetServerCallback (OnServerEvent);
	_FtpServer.SetUserCallback (OnUserEvent);
	_FtpServer.SetClientCallback (OnClientEvent);

	_FtpServer.SetMaxPasswordTries (3);

	_FtpServer.SetNoLoginTimeout (45);		// seconds
	_FtpServer.SetNoTransferTimeout (90);	// seconds
	_FtpServer.SetDataPortRange (2000, 900);	// data TCP-Port range = [100-999]
	//FtpServer.SetCheckPassDelay( 500 ); 	// milliseconds. Bruteforcing protection.


	addUser( "test","pass",CurrentApplicationConfig.FolderPathTmp, true);
	
	pthread_create(&_Listenner, NULL, &listenner, ( void * ) this );
}

void Ftp::addUser( string name, string password, string path, bool test ){
	cout << " Try Add User: " << name << ":" << password << ":" << path << endl;
	CFtpServer::CUserEntry * pUser =
	_FtpServer.AddUser (name.c_str() , password.c_str() , path.c_str() );
	
	if ( pUser ){
		if ( ! test ) 
		pUser->SetPrivileges ( /*CFtpServer::READFILE |*/ CFtpServer::WRITEFILE 
				  /*| CFtpServer::LIST | CFtpServer::DELETEFILE*/ |
				  CFtpServer::CREATEDIR  /* | CFtpServer::DELETEDIR*/) ;
		ftpUser *e = ( ftpUser*) malloc ( sizeof( ftpUser ));
		sprintf(e->name,"%s",name.c_str());
		e->pUser = pUser;
		_Users.push_front( e );
	}
	else cout << "addUser failed " << endl;
}

void Ftp::deleteUser ( string name ){
	for ( list<ftpUser*>::iterator it = _Users.begin(); it != _Users.end() ; it++ ) {
		ftpUser *f = *it;
		if ( f != NULL && name.compare( f->name ) == 0 ){
			_FtpServer.DeleteUser ( f->pUser);
			_Users.remove(f);
			LOGGER_VERB("FTP: Delete User OK");
			return;
		} 
	}
}
void* Ftp::listenner( void *data){
	Ftp* ftp = (Ftp*) data;

	if ( ftp->_FtpServer.StartListening ( INADDR_ANY, ftp->_Port ))
	{
		LOGGER_INFO ("FTP : Server is listening on " << ftp->_Port );

		if ( ftp->_FtpServer.StartAccepting ()) {
			LOGGER_DEBUG ("FTP : Server successfuly started !");
			for (;;)
				sleep (1);
		}
		else
			LOGGER_ERROR ("FTP : Unable to accept incoming connections");
		ftp->_FtpServer.StopListening ();
	}
	else {
		LOGGER_ERROR("FTP : Unable to listen");
		perror ("Listen Error : ");
	}
	LOGGER_INFO("FTP : Server stop");

	return NULL;
}


void OnServerEvent (int Event){
	switch (Event)

	{

		case CFtpServer::START_LISTENING:

			LOGGER_VERB("FTP : Server is listening !");

			break;


		case CFtpServer::START_ACCEPTING:

			LOGGER_VERB("FTP: Server is accepting incoming connexions !");

			break;


		case CFtpServer::STOP_LISTENING:

			LOGGER_VERB("FTP: Server stopped listening !");

			break;


		case CFtpServer::STOP_ACCEPTING:

			LOGGER_VERB("Server stopped accepting incoming connexions !");

			break;

		case CFtpServer::MEM_ERROR:

			LOGGER_WARN("Warning, the CFtpServer class could not allocate memory !");

			break;

		case CFtpServer::THREAD_ERROR:

			LOGGER_WARN("Warning, the CFtpServer class could not create a thread !");

			break;

		case CFtpServer::ZLIB_VERSION_ERROR:

			LOGGER_WARN("Warning, the Zlib header version differs from the Zlib library version !");

			break;

		case CFtpServer::ZLIB_STREAM_ERROR:

			LOGGER_WARN("Warning, error during compressing/decompressing data !");

			break;

	}
}


void OnUserEvent (int Event, CFtpServer::CUserEntry * pUser, void *pArg){



	switch (Event)

	{

		case CFtpServer::NEW_USER:

			LOGGER_DEBUG("FTP: A new user has been created");
					//"\tLogin: %s\r\n" "\tPassword: %s\r\n"
					//"\tStart directory: %s\r\n",
					//pUser->GetLogin (),
					//pUser->GetPassword (), pUser->GetStartDirectory ());
			break;


		case CFtpServer::DELETE_USER:

			LOGGER_VERB("FTP: User is being deleted:");//, pUser->GetLogin ());

			break;

	}
}


void OnClientEvent (int Event, CFtpServer::CClientEntry * pClient, void *pArg){



	switch (Event)

	{

		case CFtpServer::NEW_CLIENT:

			LOGGER_VERB("A new client has been created");
					//"\tClient IP: [%s]\r\n\tServer IP: [%s]\r\n",
					//inet_ntoa (*pClient->GetIP ()),
					//inet_ntoa (*pClient->GetServerIP ()));
			break;


		case CFtpServer::DELETE_CLIENT:

			LOGGER_VERB("A client is being deleted.");

			break;


		case CFtpServer::CLIENT_AUTH:

			LOGGER_VERB("A client has logged-in as: " << pClient->GetUser ()->GetLogin ());

			break;


		case CFtpServer::CLIENT_SOFTWARE:

			LOGGER_VERB("A client has proceed the CLNT FTP command: " << (char *) pArg);

			break;


		case CFtpServer::CLIENT_DISCONNECT:

			LOGGER_VERB("A client has disconnected.");

			break;


		case CFtpServer::CLIENT_UPLOAD:

			LOGGER_VERB("A client logged-on as " << pClient->GetUser ()->GetLogin () 
					<< " is uploading a file: " << (char *) pArg);

			break;


		case CFtpServer::CLIENT_DOWNLOAD:

			LOGGER_WARN("A client logged-on as " << pClient->GetUser ()->GetLogin ()
				<<  " is downloading a file: "<<  (char *) pArg);

			break;


		case CFtpServer::CLIENT_LIST:

			LOGGER_WARN("A client logged-on as " <<  pClient->GetUser ()->GetLogin ()
				<<  " is listing a directory: " << (char *) pArg);

			break;


		case CFtpServer::CLIENT_CHANGE_DIR:

			LOGGER_DEBUG("A client logged-on as " << pClient->GetUser ()->GetLogin ()
				<<   " has changed its working directory");

				 //"\tFull path: \"%s\"\r\n\tWorking directory: \"%s\"\r\n",

				 //, (char *) pArg,
				 //pClient->GetWorkingDirectory ());

			break;


		case CFtpServer::RECVD_CMD_LINE:

			printf("Received: %s (%s)>  %s\r\n",

					pClient->GetUser ()? pClient->
					GetUser ()->GetLogin () : "(Not logged in)",

					inet_ntoa (*pClient->GetIP ()),
					(char *) pArg);

			break;


		case CFtpServer::SEND_REPLY:

			printf ("* Sent: %s (%s)>  %s\r\n",

					pClient->GetUser ()? pClient->
					GetUser ()->GetLogin () : "(Not logged in)",

					inet_ntoa (*pClient->GetIP ()),
					(char *) pArg);
			break;


		case CFtpServer::TOO_MANY_PASS_TRIES:

			printf ("* Too many pass tries for (%s)\r\n",

					inet_ntoa (*pClient->GetIP ()));

			break;

	}
}

