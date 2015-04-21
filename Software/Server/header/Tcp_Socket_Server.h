//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __Tcp_Socket_Server__
#define __Tcp_Socket_Server__

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <iostream>
#include <pthread.h>


#include "Tcp.h"
#include "Recorder.h"
typedef unsigned char BYTE ;




using namespace std;



class Tcp_Socket_Server  {
public:
    Tcp_Socket_Server( int port );
    void start( );
    static int getPort();

    static Tcp_Socket_Server* tcp_socket_server;
protected:

private:
	static void* listenner( void* data );
	//list<pthread_t> _RecordersThreads;
	pthread_t _ThreadListenner;
    static int _Port ;
};

#endif /* defined( __Tcp_Socket_Server__ ) */
