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

typedef unsigned char BYTE ;


class Udp  {
public:
    Udp( int port );
    void listenner();

    bool sendFrame( void* so, BYTE* data, int size );
protected:
private:
    static BYTE* _IPSrv;
    int _Port ;
};

#endif /* defined(__server__Udp__) */
