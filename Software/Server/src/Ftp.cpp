//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "../header/Config.h"
#include "../header/Ftp.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define BUFFSIZE 2000


using namespace std;

CFtpServer _FtpServer;

Ftp::Ftp( int port) {
	_Port = port;

	_FtpServer.SetServerCallback (OnServerEvent);
	_FtpServer.SetUserCallback (OnUserEvent);
	_FtpServer.SetClientCallback (OnClientEvent);

	_FtpServer.SetMaxPasswordTries (3);

	_FtpServer.SetNoLoginTimeout (45);		// seconds
	_FtpServer.SetNoTransferTimeout (90);	// seconds
	_FtpServer.SetDataPortRange (2000, 900);	// data TCP-Port range = [100-999]
	//FtpServer.SetCheckPassDelay( 500 ); 	// milliseconds. Bruteforcing protection.



	pthread_create(&_Listenner, NULL, &listenner, ( void * ) this );
}

void Ftp::addUser( string name, string password, string path ){
	CFtpServer::CUserEntry * pUser =
	_FtpServer.AddUser (name.c_str() , password.c_str() , path.c_str() );

	if ( pUser ){
		pUser->SetPrivileges (/*CFtpServer::READFILE |*/ CFtpServer::WRITEFILE 
				/*CFtpServer::LIST | CFtpServer::DELETEFILE |
				  CFtpServer::CREATEDIR | CFtpServer::DELETEDIR*/);
	}
}

void* Ftp::listenner( void *data){
	Ftp* ftp = (Ftp*) data;

	if ( _FtpServer.StartListening ( INADDR_ANY, ftp->_Port ))
	{
		LOGGER_INFO ("FTP : Server is listening on " << ftp->_Port );

		if (_FtpServer.StartAccepting ()) {
			LOGGER_DEBUG ("FTP : Server successfuly started !");
			for (;;)
				sleep (1);
		}
		else
			LOGGER_ERROR ("FTP : Unable to accept incoming connections");
		_FtpServer.StopListening ();
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

			printf ("* Server is listening !\r\n");

			break;


		case CFtpServer::START_ACCEPTING:

			printf ("* Server is accepting incoming connexions !\r\n");

			break;


		case CFtpServer::STOP_LISTENING:

			printf ("* Server stopped listening !\r\n");

			break;


		case CFtpServer::STOP_ACCEPTING:

			printf ("* Server stopped accepting incoming connexions !\r\n");

			break;

		case CFtpServer::MEM_ERROR:

			printf
				("* Warning, the CFtpServer class could not allocate memory !\r\n");

			break;

		case CFtpServer::THREAD_ERROR:

			printf
				("* Warning, the CFtpServer class could not create a thread !\r\n");

			break;

		case CFtpServer::ZLIB_VERSION_ERROR:

			printf
				("* Warning, the Zlib header version differs from the Zlib library version !\r\n");

			break;

		case CFtpServer::ZLIB_STREAM_ERROR:

			printf
				("* Warning, error during compressing/decompressing data !\r\n");

			break;

	}
}


void OnUserEvent (int Event, CFtpServer::CUserEntry * pUser, void *pArg){



	switch (Event)

	{

		case CFtpServer::NEW_USER:

			printf ("* A new user has been created:\r\n"
					"\tLogin: %s\r\n" "\tPassword: %s\r\n"
					"\tStart directory: %s\r\n",
					pUser->GetLogin (),
					pUser->GetPassword (), pUser->GetStartDirectory ());

			break;


		case CFtpServer::DELETE_USER:

			printf ("* \"%s\"user is being deleted: \r\n", pUser->GetLogin ());

			break;

	}
}


void OnClientEvent (int Event, CFtpServer::CClientEntry * pClient, void *pArg){



	switch (Event)

	{

		case CFtpServer::NEW_CLIENT:

			printf ("* A new client has been created:\r\n"
					"\tClient IP: [%s]\r\n\tServer IP: [%s]\r\n",

					inet_ntoa (*pClient->GetIP ()),
					inet_ntoa (*pClient->GetServerIP ()));

			break;


		case CFtpServer::DELETE_CLIENT:

			printf ("* A client is being deleted.\r\n");

			break;


		case CFtpServer::CLIENT_AUTH:

			printf ("* A client has logged-in as \"%s\".\r\n",
					pClient->GetUser ()->GetLogin ());

			break;


		case CFtpServer::CLIENT_SOFTWARE:

			printf ("* A client has proceed the CLNT FTP command: %s.\r\n",
					(char *) pArg);

			break;


		case CFtpServer::CLIENT_DISCONNECT:

			printf ("* A client has disconnected.\r\n");

			break;


		case CFtpServer::CLIENT_UPLOAD:

			printf
				("* A client logged-on as \"%s\" is uploading a file: \"%s\"\r\n",

				 pClient->GetUser ()->GetLogin (), (char *) pArg);

			break;


		case CFtpServer::CLIENT_DOWNLOAD:

			printf
				("* A client logged-on as \"%s\" is downloading a file: \"%s\"\r\n",

				 pClient->GetUser ()->GetLogin (), (char *) pArg);

			break;


		case CFtpServer::CLIENT_LIST:

			printf
				("* A client logged-on as \"%s\" is listing a directory: \"%s\"\r\n",

				 pClient->GetUser ()->GetLogin (), (char *) pArg);

			break;


		case CFtpServer::CLIENT_CHANGE_DIR:

			printf
				("* A client logged-on as \"%s\" has changed its working directory:\r\n"

				 "\tFull path: \"%s\"\r\n\tWorking directory: \"%s\"\r\n",

				 pClient->GetUser ()->GetLogin (), (char *) pArg,
				 pClient->GetWorkingDirectory ());

			break;


		case CFtpServer::RECVD_CMD_LINE:

			printf ("* Received: %s (%s)>  %s\r\n",

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

