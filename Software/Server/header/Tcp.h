//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __h__TCP__
#define __h__TCP__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Communication.h"
//#include "Recorder.h"

typedef unsigned char BYTE ;

using namespace std;

class Tcp  {
	public:
		Tcp( int socket, void* recorder );
		int getSocket();
		void sendTcp( BYTE* data, int size, bool needAck);
		void start();
		//void stop();
		void sendAck( BYTE* Command);
		Communication *_Communication;
	protected:

	private:
		static void* sends( void *);
		static void* listen( void *);
		pthread_t _Listenner, _Sender;
		void *_Recorder;	
		int _Socket ;
};
#endif /* defined( __TCP__ ) */
