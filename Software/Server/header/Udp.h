//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Udp__
#define __server__Udp__

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef unsigned char BYTE ;


class Udp  {
public:
    Udp( int port );
    static void *listenner( void* data);
    bool sendFrame( void* so, BYTE* data, int size );
protected:
private:
    pthread_t _Listenner;
    static BYTE* _IPSrv;
    int _Port, _PortTcp, _Socket ;
	struct sockaddr_in _Client;
};

#endif /* defined(__server__Udp__) */
