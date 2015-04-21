//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Udp__
#define __server__Udp__

#include <stdio.h>
#include "Communication.h"

typedef struct InfoTCP{
    char ipAddress[20];
    int  port;
}InfoTCP;

typedef unsigned char BYTE ;


class Udp : Communication {
public:
    Udp( string iface , int port ) ;
    Udp( int port );
    Udp( );

    void sendImage();
    InfoTCP* getInfoSrv();

protected:
private:
	static Udp *_udp;
	static InfoTCP * _TcpInfo;
	static void ansInfoSrv ( BYTE data[], int size );
	void send ( BYTE* data, int size, char*ipDest );
    string _interface ;
    int _Port ;
};

#endif /* defined(__server__Udp__) */
