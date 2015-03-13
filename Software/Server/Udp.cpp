//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Udp.h"
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include "Recorder.h"

using namespace std;

#define BUFFSIZE 128

Udp::Udp( int port ) {
    _Port = port;
}


void Udp::listenner(){


    int sock;
    struct sockaddr_in echoserver;
    struct sockaddr_in echoclient;
    char buffer[BUFFSIZE];
    int clientlen, serverlen, received = 0;



    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        cout << "Failed to create socket" << endl;
        return;
    }



    memset(&echoserver, 0, sizeof(echoserver));
    echoserver.sin_family = AF_INET;
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);
    echoserver.sin_port = htons( _Port );
    serverlen = sizeof(echoserver);
    bind(sock, (struct sockaddr *) &echoserver, serverlen);// {


    while (1) {
        clientlen = sizeof(echoclient);

        if ( (received = recvfrom(sock, buffer, BUFFSIZE, 0, (struct sockaddr *) &echoclient, (socklen_t*) &clientlen)) < 0) {
            cout << "Failed to receive message" << endl;
            continue;
        }

        char* ipClient = inet_ntoa(echoclient.sin_addr);
        printf( "Client connected: %s: Data(%i) ==>  ", ipClient, received);

        Recorder *r = Recorder::findRecorderByIp(ipClient);

        if ( r == NULL ){
            cout << "First Frame" << endl;
            r = new Recorder(this, ipClient);
        }
        else cout << "Already get Frame" << endl;

        r->getFrameUdp((BYTE*)buffer, received );
        //for ( int i = 0; i < received ; i++) printf("%02X ", buffer[i] & 0xFF );

        char *data = "e\0";
        sendto(sock, data, sizeof(data), 0, (struct sockaddr *) &echoclient, sizeof(echoclient));
    }
    return;
}

