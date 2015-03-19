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
    this->_CommunicationUdp = new Communication( 0x10, 0x22, 0x20 );
    _CommunicationUdp->setFunction((FuncType)&(Recorder::getMacAddress), (int)GET_MAC_ADDR);


    this->_IpAddr = ( char *) malloc ( strlen(ip) );
    memcpy( _IpAddr,ip,strlen(ip) );

    _Recorders.push_back(this);
}

void Recorder::getMacAddress( BYTE* data, int size, void *sender ){
     Recorder *rec = (Recorder*) sender;
     cout << "Get Mac Addr" << "On Ip:" ;
     fflush(stdout);
     if ( rec != NULL ) printf("%s\n", rec->_IpAddr);
}

void Recorder::getFrameUdp( BYTE* data, int size  ){
    _CommunicationUdp->recieveData(data, size, (void*) this);
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
