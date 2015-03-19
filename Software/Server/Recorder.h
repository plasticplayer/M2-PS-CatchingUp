//
//  Recorder.h
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Recorder__
#define __server__Recorder__

#include <stdio.h>
#include "Udp.h"

#include "Communication.h"

typedef unsigned char BYTE ;


class Recorder{
public:
    Recorder ( Udp *udp, char* ip );
    void getFrameUdp( BYTE* data, int size );
    static Recorder* findRecorderByIp( char* ip );

private:
    static list<Recorder *> _Recorders;
    static void getMacAddress( BYTE* data, int size, void *sender);
protected:
    Communication *_CommunicationUdp;
    char *_IpAddr;
    Udp *_UdpSrv;
    BYTE _MacAddress[6];
};

#endif /* defined(__server__Recorder__) */
