//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__FTP__
#define __server__FTP__

#include <list>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "CFtpServer.h"
typedef unsigned char BYTE ;

typedef struct ftpUser {
	char name[12];
	CFtpServer::CUserEntry * pUser;
}ftpUser;

using namespace std;

class Ftp  {
public:
    	Ftp( int port );
	static Ftp* _Ftp;
	void addUser( string name, string password, string path , bool test);
    	void deleteUser ( string name );
	static void *listenner( void* data);
protected:
private:
	list<ftpUser *> _Users ;
	int _Port;
	CFtpServer _FtpServer;
    	pthread_t _Listenner;
};


void OnServerEvent (int Event);
void OnUserEvent (int Event, CFtpServer::CUserEntry * pUser, void *pArg);
void OnClientEvent (int Event, CFtpServer::CClientEntry * pClient, void *pArg);

#endif /* defined(__server__Udp__) */
