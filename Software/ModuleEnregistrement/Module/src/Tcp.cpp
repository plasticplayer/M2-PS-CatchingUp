
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//
#include "Config.h"
#include "logger.h"
#include "Tcp.h"
#include "Udp.h"
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <fstream>

using namespace std;

#define SIZE_BUFFER 1024

Tcp *Tcp::_tcp = NULL ;


Tcp::Tcp( InfoTCP* infoConnexion ) : Communication(0x10, 0x22, 0x20 ) {
	LOGGER_VERB("Start Tcp on: " << infoConnexion->ipAddress << ":" << infoConnexion->port);
	//Create socket
	_Sock = socket(AF_INET , SOCK_STREAM , 0);
	if (_Sock == -1) perror("Could not create socket");

	_Server.sin_addr.s_addr = inet_addr( infoConnexion->ipAddress  );
	_Server.sin_family = AF_INET;
	_Server.sin_port = htons( infoConnexion->port );

	_tcp = this;
}
int Tcp::getSocket(){
	return _Sock;
}
void* Tcp::listen ( void * data ){
	Tcp* tcp = (Tcp*) data;
	int read_size;
	char client_message[SIZE_BUFFER];

	//Receive a message from client
	while( (read_size = recv( tcp->getSocket() , client_message , SIZE_BUFFER , 0)) > 0 )
	{
		LOGGER_VERB( "Tcp get Frame of " << read_size << " elements");
		tcp->recieveData((BYTE*)client_message, read_size );
	}
	LOGGER_DEBUG ( "Tcp close socket" );

	fflush(stdout);
	return NULL;
}

void Tcp::sendData( BYTE* data, int size , bool needAck ){
	LOGGER_VERB("TCP -> " << size );
	// Send Frames
	BYTE* Datas = NULL;
	int res = encodeData(data,&Datas,(unsigned long)size);
	
	if( send(_Sock , (char*)Datas , res ,0) < 0)
	{
		perror("Send failed : ");
		free(Datas);
		return;
	}
	free(Datas);
}

void Tcp::start(){
	//Connect to remote server
	if (connect( _Sock , (struct sockaddr *) &_Server , sizeof( _Server )) < 0)
	{
		LOGGER_ERROR("TCP: Connect failed");
		return;
	}
	pthread_create( &_Listenner , NULL ,  &(this->listen) , (void*) this );
}
