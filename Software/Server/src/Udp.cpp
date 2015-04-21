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
#include "../header/Recorder.h"

#define BUFFSIZE 2000


using namespace std;



Udp::Udp( int port) {
	_Port = port;
}



bool Udp::sendFrame( void* so, BYTE* data, int size ){
	struct sockaddr_in *des  = (struct sockaddr_in *) so;
	//char* ip = inet_ntoa( des->sin_addr );

	cout << "Write:";
	//for ( int i = 0; i < size; i++ ) printf(" %02x", data[i] );
	//cout << " ==> " << ip << ":" << endl;


	int v = sendto( _Socket , data, size, 0, (struct sockaddr *) des, sizeof( struct sockaddr ));
	//int v = sendto( _Socket , data, size, 0, (struct sockaddr *) &_Client, sizeof( struct sockaddr ));

	return ( v == size );
}


void Udp::listenner(){


//	int sock;
	struct sockaddr_in echoserver;
	struct sockaddr_in echoclient;
	char *buffer = ( char *) malloc( sizeof(char) * BUFFSIZE) ;
	memset(buffer,0,BUFFSIZE);
	int clientlen, serverlen, received = 0;



	if ((_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		cout << "Failed to create socket" << endl;
		return;
	}


	// Ecoute sur le port 1902
	memset(&echoserver, 0, sizeof(echoserver));
	memset(&_Client, 0, sizeof(_Client));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY);
	echoserver.sin_port = htons( _Port );
	serverlen = sizeof(echoserver);

	bind(_Socket, (struct sockaddr *) &echoserver, serverlen);

	
	char ipClient[20];
	while (1) {
		clientlen = sizeof(echoclient);

		if ( (received = recvfrom(_Socket, buffer, BUFFSIZE, 0, (struct sockaddr *) &echoclient, (socklen_t*) &clientlen)) < 0)  {
			cout << "Failed to receive message" << endl;
			continue;
		}
		
		strcpy(ipClient,inet_ntoa(echoclient.sin_addr));
		
		printf( "Udp: Client connected: %s: Data(%i) ==>  ", ipClient, received);

		Recorder *r = Recorder::findRecorderByIp(ipClient);

		if ( r == NULL ){
			r = new Recorder(this, ipClient);
		}

		r->setUdpSocket( (void*) &echoclient, sizeof(echoclient) );
		
		r->getFrameUdp((BYTE*)buffer, received);
	}

	free( buffer );
	return;
}


