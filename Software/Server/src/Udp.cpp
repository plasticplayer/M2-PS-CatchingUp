//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//


#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include "../header/Udp.h"
#include "../header/Config.h"
#include "../header/Recorder.h"

#define BUFFSIZE 2000


using namespace std;



Udp::Udp( int port) {
	_Port = port;
	pthread_create(&_Listenner, NULL, &listenner, ( void * ) this );
}


bool Udp::sendFrame( void* so, BYTE* data, int size ){
	struct sockaddr_in *des  = (struct sockaddr_in *) so;
	/*
	   char* ip = inet_ntoa( des->sin_addr );
	   cout << "Write:";
	   for ( int i = 0; i < size; i++ ) printf(" %02x", data[i] );
	   cout << " ==> " << ip << ":" << endl;
	 */
	int v = sendto( _Socket , data, size, 0, (struct sockaddr *) des, sizeof( struct sockaddr ));

	return ( v == size );
}


void* Udp::listenner( void *data){
	Udp* udp = (Udp*) data;
	LOGGER_DEBUG("Udp Load Server: " << udp->_Port);
	//	int sock;
	struct sockaddr_in echoserver;
	struct sockaddr_in echoclient;
	char *buffer = ( char *) malloc( sizeof(char) * BUFFSIZE) ;
	memset(buffer,0,BUFFSIZE);
	int clientlen, serverlen, received = 0;



	if ((udp->_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		LOGGER_ERROR("Udp : Failed to create socket");
		return NULL;
	}


	memset(&echoserver, 0, sizeof(echoserver));
	memset(&(udp->_Client), 0, sizeof(udp->_Client));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY);
	echoserver.sin_port = htons( udp->_Port );
	serverlen = sizeof(echoserver);

	bind(udp->_Socket, (struct sockaddr *) &echoserver, serverlen);

	char tmp[250];	
	char ipClient[20];
	LOGGER_DEBUG( "Udp : Listenner Ok " );
	usleep( 100 );
	while ( true ) {
		clientlen = sizeof(echoclient);

		if ( (received = recvfrom(udp->_Socket, buffer, BUFFSIZE, 0, (struct sockaddr *) &echoclient, (socklen_t*) &clientlen)) < 0)  {
			LOGGER_ERROR("Udp : Failed to receive message");
			continue;
		}

		strcpy(ipClient,inet_ntoa(echoclient.sin_addr));

		memset(tmp,0,sizeof(tmp));
		//for(int i = 0; i <  ( received > 10 ) ? 10 : received ; i++) sprintf(tmp,"%s %02X",tmp, buffer[i]);

		//printf( "Udp: %s:%s \n", ipClient,tmp);

		Recorder *r = Recorder::findRecorderByIp(ipClient);

		if ( r == NULL ){
			r = new Recorder(udp, ipClient);
		}

		r->setUdpSocket( (void*) &echoclient, sizeof(echoclient) );

		r->getFrameUdp((BYTE*)buffer, received);
	}

	free( buffer );
	return NULL;
}


