//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __h__TCP__
#define __h__TCP__
#include "Config.h"
#include <string.h>
#include <iostream>
#include "define.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Communication.h"

#ifdef SSL_ENABLE
#include "openssl/ssl.h"
#include "openssl/err.h"
#endif

typedef unsigned char BYTE ;

typedef void (*FuncDeco)( void* recorder );
using namespace std;

class Tcp  {
	public:
		#ifdef SSL_ENABLE
		Tcp( int socket, void* recorder , SSL *ssl);
		#else
		Tcp( int socket, void* recorder );
		#endif
		
		FuncDeco deco[1];
		int getSocket();
		void sendTcp( BYTE* data, int size, bool needAck);
		void start();
		//void stop();
		void sendAck( BYTE* Command);
		Communication *_Communication;
	protected:

	private:
		#ifdef SSL_ENABLE
		SSL *_Ssl;
		#endif
		static void* sends( void *);
		static void* listen( void *);
		pthread_t _Listenner, _Sender;
		void *_Recorder;	
		int _Socket ;
};
#endif /* defined( __TCP__ ) */
