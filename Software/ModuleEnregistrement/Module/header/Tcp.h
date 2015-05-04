//
//  Tcp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Tcp__
#define __server__Tcp__

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "logger.h"
#include "Communication.h"



typedef unsigned char BYTE ;


class Tcp : public Communication {
public:
   	Tcp( InfoTCP* infoConnexion );
   	void sendData( BYTE* data, int size, bool needAck );
	void start();
	int getSocket();
	static Tcp* _tcp;	
protected:


private:
	static void *listen ( void * data );
	int _Sock;
	pthread_t _Listenner;
	struct sockaddr_in _Server;
};

#endif /* defined(__server__Tcp__) */