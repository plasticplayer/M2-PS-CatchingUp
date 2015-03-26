//
//  Recorder.cpp
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "Recorder.h"
#include "Communication.h"

list<Recorder *> Recorder::_Recorders;
using namespace std;


Recorder::Recorder ( Udp *udp, char* ip ){
  //  if ( Recorder::_Recorders == NULL )
   //     _Recorders = new list<Recorder *>();

    this->_UdpSrv = udp;
    this->_UdpSocket = NULL;
    this->_CommunicationUdp = new Communication( 0x10, 0x22, 0x20 );
    _CommunicationUdp->setFunction((FuncType)&(this->getMacAddress), (int)GET_MAC_ADDR);


    this->_IpAddr = ( char *) malloc ( strlen(ip) );
    memcpy( _IpAddr,ip,strlen(ip) );

    _Recorders.push_back(this);
}

void Recorder::getMacAddress( BYTE* data, int size, void *sender ){
     Recorder *rec = (Recorder*) sender;

     cout << "Get Mac Addr: ";

     for (int i = 0; i < size; i++ ) printf("%c", data[i] );

     cout << " On Ip: " << rec->_IpAddr << endl;

     fflush(stdout);


     BYTE req[] = { 0x42, TCP_PORT>>8, TCP_PORT&0xFF };
     BYTE* ans = NULL;
     int t = rec->_CommunicationUdp->encodeData(req,&ans, sizeof(req));

     rec->sendUdpFrame(ans, t);
}

void Recorder::getFrameUdp( BYTE* data, int size  ){
    for (int i = 0; i < size; i++ ) printf("%02x ", data[i] );
    _CommunicationUdp->recieveData(data, size, (void*) this);
}

void Recorder::setUdpSocket ( void* sock, int size ){
    if ( this->_UdpSocket != NULL )
        free ( this->_UdpSocket );

    this->_UdpSocket = malloc( size );
    memcpy( this->_UdpSocket , sock, size );

}

bool Recorder::sendUdpFrame( BYTE* data, int size ){
    return this->_UdpSrv->sendFrame(_UdpSocket, data, size);
}

Recorder* Recorder::findRecorderByIp( char* ip ){
    if ( _Recorders.empty() )
        return NULL;

    for ( list<Recorder*>::iterator it = _Recorders.begin(); it != _Recorders.end(); ++it ){
        Recorder *rec = *it;
        if ( strcmp(ip,rec->_IpAddr) == 0 )
            return rec;
    }
    return NULL;
}
