//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "../header/Tcp_Socket_Server.h"
#include "../header/Tcp.h"
#include "../header/Config.h"
#include "../header/define.h"
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


#define BUFFSIZE 256


using namespace std;




Tcp_Socket_Server* Tcp_Socket_Server::tcp_socket_server = NULL ;
int Tcp_Socket_Server::_Port ;

Tcp_Socket_Server::Tcp_Socket_Server( int port ) {
	_Port = port;
	tcp_socket_server = this;
}

int Tcp_Socket_Server::getPort(){
	if ( tcp_socket_server == NULL )
		return -1;

	else return _Port;
}

void Tcp_Socket_Server::start(){

	if( pthread_create( &_ThreadListenner , NULL ,  &(this->listenner) , (void*) this ) < 0)
	{
		LOGGER_ERROR("TCP : Error: could not create thread");
	}
}




void* Tcp_Socket_Server::listenner( void* data){
	int socket_desc , client_sock , c;
	struct sockaddr_in server , client;


	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		LOGGER_ERROR("TCP : Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( _Port );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		LOGGER_ERROR("Tcp bind failed. Error");
		return NULL;
	}
	LOGGER_DEBUG("TCP : Bind done");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	LOGGER_DEBUG("Tcp : Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);


	Recorder *r;

	while( ( client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		LOGGER_DEBUG("Connection accepted");

		r = Recorder::findRecorderByIp( inet_ntoa(client.sin_addr) );

		if ( r == NULL ){
			continue;
		}

		Tcp *client = new Tcp(  client_sock , r );
		r->setTcpSocket(client);
		client->start();
		//return NULL;
	}
	return NULL;
}
