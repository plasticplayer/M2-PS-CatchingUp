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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Udp.h"

#include "Communication.h"

typedef unsigned char BYTE ;

const ulong TCP_PORT = 10101;

class Recorder{
public:
    Recorder ( Udp *udp, char* ip );
    void getFrameUdp( BYTE* data, int size );
    void setUdpSocket ( void* sock,  int size );
    static Recorder* findRecorderByIp( char* ip );

private:
    static list<Recorder *> _Recorders;
    static void getMacAddress( BYTE* data, int size, void *sender);
protected:
    bool sendUdpFrame( BYTE* data , int size);
    Communication *_CommunicationUdp;
    char *_IpAddr;
    Udp *_UdpSrv;
    void* _UdpSocket;
    BYTE _MacAddress[13];
};

#endif /* defined(__server__Recorder__) */
