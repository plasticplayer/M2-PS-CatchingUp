//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "../header/Tcp.h"
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

#define MAX_SIZE_UDP 1024

Tcp *Tcp::_tcp = NULL ;


Tcp::Tcp( InfoTCP* infoConnexion ) : Communication(0x10, 0x22, 0x20 ) {
	cout << "Start Tcp on: " << infoConnexion->ipAddress << ":" << infoConnexion->port << endl;
	//Create socket
	_Sock = socket(AF_INET , SOCK_STREAM , 0);
	if (_Sock == -1) perror("Could not create socket");

	cout<<"Socket created\n";
	_Server.sin_addr.s_addr = inet_addr( infoConnexion->ipAddress  );
	_Server.sin_family = AF_INET;
	_Server.sin_port = htons( infoConnexion->port );

	//Connect to remote server
	if (connect( _Sock , (struct sockaddr *) &_Server , sizeof( _Server )) < 0)
	{
		perror("connect failed. Error");
		return;
	}

	cout << "Connect" << endl;

	string data = "GET / HTTP/1.1\r\n\r\n";
	if( send(_Sock , data.c_str() , strlen( data.c_str() ) , 0) < 0)
	{
		perror("Send failed : ");
		return;
	}
	cout<<"Data send\n";

	int read_size;
	char client_message[2000];


	//Receive a message from client
	// read_size = recv( _Sock , client_message , 2000 , 0);
	//    cout << "aze " << read_size << endl;

	//   sleep(1000);

	//setFunction( (FuncType) &ansInfoSrv, (int)GET_INFO_SRV );
	_tcp = this;
}


void Tcp::start(){

}
