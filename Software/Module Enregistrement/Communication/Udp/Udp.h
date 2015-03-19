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
    char* ipAddress;
    int   port;
}InfoTCP;

typedef unsigned char BYTE ;


class Udp : Communication {
public:
    Udp( int port );
    Udp( );

    void listenner();

    InfoTCP* getInfoSrv( int port );

protected:
private:
    int _Port ;
};

#endif /* defined(__server__Udp__) */
