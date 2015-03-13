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
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

using namespace std;

#define BUFFSIZE 128

int getMac(char *iface, char *dest){
	// Copie l'adresse mac de l'interface iface dans dest
    struct ifreq s;
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

    strcpy(s.ifr_name, iface);
    if (fd >= 0 && 0 == ioctl(fd, SIOCGIFHWADDR, &s)){
        strcpy( dest, s.ifr_addr.sa_data);
        return 1;
    }
    return 0;
}

Udp::Udp( int port ) : Communication(0x10, 0x22, 0x20 ) {
    _Port = port;
}
Udp::Udp( ) : Communication(0x10, 0x22, 0x20 ) {
    _Port = -1;
}

InfoTCP* Udp::getInfoSrv( int port ){
    // Declare TimeOut
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 000000;

    // Get Mac ADDRESS
	char mac[7];
	memset((void*)mac,sizeof(mac),0);
    mac[0] = (char) GET_MAC_ADDR;
	if( getMac((char *)"eth0",mac+1 ) == 0 ){
        cout << "Interface not found" << endl;
        return NULL;
	}

    BYTE* data = NULL;
    for ( int i = 0; i < 7; i++ )
        printf("%02x ", mac[i] & 0xFF );

    printf("\n");
    int sizeData = encodeData( (BYTE*) mac , &data, sizeof(mac) );
    for ( int i = 0; i < sizeData; i++ )
        printf("%02x ", data[i] );

	int sock;
	char buffer[BUFFSIZE];
	unsigned int clientlen;
	int received = 0;
	struct sockaddr_in echoserver, echoclient;

	memset((void*)&echoclient,0,sizeof(echoclient));



	/* Create the UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		cout << "Failed to create socket" << endl ;
		return NULL;
	}


	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));                 /* Clear struct */
	echoserver.sin_family = AF_INET;                            /* Internet/IP */
	echoserver.sin_addr.s_addr = inet_addr("255.255.255.255");  /* IP address */
	echoserver.sin_port = htons( port );                        /* server port */


	/* Set socket option to broadcast*/
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(int));

	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		cout << "Error affecting Time Out" << endl;
	}

    InfoTCP* info  = NULL;
    // Renvoie 10 fois la requetes si pas de réponse
	for ( int i = 0; i < 10; i++ ){

		/* Send the word to the server */
		if (sendto(sock, (char*) data, sizeData, 0, (struct sockaddr *) &echoserver, sizeof(echoserver)) != sizeData) {
			printf("Mismatch in number of sent bytes");
			continue;
		}


		clientlen = sizeof(echoclient);
		int res  = recvfrom(sock, buffer, BUFFSIZE, 0, (struct sockaddr *) &echoclient, (socklen_t*) &clientlen);
		if ( res > 0 ){
			printf( "Recieve From: %s\n", inet_ntoa(echoclient.sin_addr));
			info = ( InfoTCP* ) malloc( sizeof( InfoTCP ));
            info->port = 0;
            info->ipAddress = "";
			break;
		}
		else if ( res < 0 )
			printf("Time out\n");
	}
	close(sock);
	return info;
}

void Udp::listenner(){
    /*int sock;
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
    }*/
    return;
}

