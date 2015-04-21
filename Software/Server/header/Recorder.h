//
//  Recorder.h
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __h__server__Recorder__
#define __h__server__Recorder__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Communication.h"
#include "Udp.h"
#include "Tcp.h"

#include <fstream>


typedef unsigned char BYTE ;
using namespace std;
class Recorder{
public:
    Recorder ( Udp *udp, char* ip );
//	~Recorder();
    void getFrameUdp( BYTE* data, unsigned long size );
    void setUdpSocket ( void* sock,  int size );
    static Recorder* findRecorderByIp( char* ip );
    
    bool isTcpConnected();
    void setTcpSocket( Tcp* s);
    

	char**_Image;
private:
    static list<Recorder *> _Recorders;
    static void getImageParts( BYTE* data, unsigned long size, void *sender );
    static void getMacAddress( BYTE* data, unsigned long size, void *sender);
    static void getImageInfo( BYTE* data, unsigned long size, void *sender );
	static void imageCompletlySend( BYTE* data, unsigned long size, void *sender );
	
protected:
    bool sendUdpFrame( BYTE* data , int size);
	Tcp *_Tcp;
    Communication *_CommunicationUdp;
    
    Udp *_UdpSrv;
    void* _UdpSocket;
    BYTE _MacAddress[13];
    char *_IpAddr;
    
    int _Count, _SizeImage;
	//int _TcpSocket;
    int* _ImageParts;
   
};

#endif /* defined(__server__Recorder__) */
