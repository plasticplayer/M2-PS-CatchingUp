
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
#include <signal.h>

using namespace std;

#define SIZE_BUFFER 1024

Tcp *Tcp::_tcp = NULL ;


#ifdef SSL_ENABLE
SSL_CTX * InitCTX(){
	SSL_CTX * ctx;
	//cout << "OpenSSL Algo" << endl;
	OpenSSL_add_all_algorithms();
	//cout << "SSL load errors" << endl;
	SSL_load_error_strings();
	//cout << "SSL CTX NEW " << endl;
	
	ctx = SSL_CTX_new((SSL_METHOD*)SSLv3_client_method());
	if ( ctx == NULL ){
		ERR_print_errors_fp(stderr);
		LOGGER_ERROR("InitCTX Failed");
		abort();
	}
	cout << "SSL INIT CTX OK" << endl;
	return ctx;
}
#endif

Tcp::Tcp( InfoTCP* infoConnexion ) : Communication(0x10, 0x22, 0x20 ) {
	_isConnected = false;
	LOGGER_VERB("Start Tcp on: " << infoConnexion->ipAddress << ":" << infoConnexion->port);
	#ifdef SSL_ENABLE
	SSL_library_init();
	_Ctx = InitCTX();	
	#endif

	//Create socket
	signal(SIGPIPE, sigpipe_handler);

	_Sock = socket(AF_INET , SOCK_STREAM , 0);
	if (_Sock == -1) perror("Could not create socket");

	_Server.sin_addr.s_addr = inet_addr( infoConnexion->ipAddress  );
	_Server.sin_family = AF_INET;
	_Server.sin_port = htons( infoConnexion->port );

	 struct timeval  timeout;
    	 timeout.tv_sec = 10;
  	 timeout.tv_usec = 0;

	 if (setsockopt ( _Sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0)
        	LOGGER_ERROR("setsockopt failed");

	/*if ( connect(_Sock, ( struct sockaddr*)&_Server,sizeof(_Server )) != 0 ){
		close ( _Sock );
		LOGGER_ERROR("Connect Failed");
		return;
	}
	#ifdef SSL_ENABLE
	//	cout << "SSL_new" << endl;
		_Ssl = SSL_new ( _Ctx );
	//	cout << "SSK set fd" << endl;
		SSL_set_fd( _Ssl, _Sock );
	//	cout << "SSL CONNECT" << endl;
		if ( SSL_connect(_Ssl) == -1 ){
			LOGGER_ERROR("Cannot Init TCP/SSL");
			return;
		}
	#endif
	*/
	_tcp = this;
}

bool Tcp::connectS(){
	if ( _isConnected )
		return true;
	LOGGER_DEBUG("Try TCP connection");
	Udp::_udp->getInfoSrv();

		
	_Sock = socket(AF_INET , SOCK_STREAM , 0);
	if (_Sock == -1) perror("Could not create socket");
	
	if ( connect(_Sock, ( struct sockaddr*)&_Server,sizeof(_Server )) != 0 ){
		close ( _Sock );
		LOGGER_ERROR("Connect Failed");
		return false;
	}

	#ifdef SSL_ENABLE
	cout << "SSL_new" << endl;
	_Ssl = SSL_new ( _Ctx );
	cout << "SSK set fd" << endl;
	SSL_set_fd( _Ssl, _Sock );
	cout << "SSL CONNECT" << endl;
	if ( SSL_connect(_Ssl) == -1 ){
		LOGGER_ERROR("Cannot Init TCP/SSL");
		return false;
	}
	#endif	
	
	_isConnected = true;
	return true;
}

int Tcp::getSocket(){
	return _Sock;
}

void* Tcp::listen ( void * data ){
	Tcp* tcp = (Tcp*) data;
	int read_size;
	char client_message[SIZE_BUFFER];
	
	for ( ;; ){
		while ( ! tcp->connectS() )
			usleep(500000);
		
		//Receive a message from client
		#ifdef SSL_ENABLE
		while( ( read_size = SSL_read( tcp->_Ssl , client_message, SIZE_BUFFER )) > 0  ) 
		#else
		while( (read_size = recv( tcp->getSocket() , client_message , SIZE_BUFFER , 0)) > 0 )
		#endif
		{
			LOGGER_VERB( "Tcp get Frame of " << read_size << " elements");
			tcp->recieveData((BYTE*)client_message, read_size );
		}
		tcp->_isConnected = false;
		close ( tcp->_Sock );
		LOGGER_DEBUG ( "Tcp close socket" );
	
	}
	return NULL;
}

void Tcp::sendData( BYTE* data, int size , bool needAck ){
	if ( !_isConnected ){
		LOGGER_ERROR("Cannot send TCP data. Socket is closed");
		return;
	}

	LOGGER_VERB("TCP -> " << size );
	// Send Frames
	BYTE* Datas = NULL;
	int res = encodeData(data,&Datas,(unsigned long)size);
	int sendTcp = -19;

	#ifndef SSL_ENABLE
	if( ( sendTcp = send(_Sock , (char*)Datas , res ,0)) < 0)
	#else
	if ( (sendTcp = SSL_write( _Ssl , (char*) Datas, res))  < 0 )
	#endif
	{
		perror("Send failed : ");
		free(Datas);
		return;
	}
	//LOGGER_DEBUG("Tcp Send end: " << sendTcp );
	free(Datas);
}

void Tcp::start(){
	pthread_create( &_Listenner , NULL ,  &(this->listen) , (void*) this );
}
