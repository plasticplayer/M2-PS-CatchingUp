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
		Tcp( int socket);

		void send();
		void start();
		//void stop();
	protected:

	private:
		static void* listen( void *);
		pthread_t _Listenner;
		Communication *_Communication;
		int _Socket ;
};
//#else
//class Tcp;
#endif /* defined( __TCP__ ) */
