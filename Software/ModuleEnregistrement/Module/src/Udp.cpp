//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//
#include "../header/Config.h"
#include "../header/logger.h"
#include "../header/Udp.h"
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
#include <fstream>

using namespace std;

#define MAX_SIZE_UDP 1024

#define BUFFSIZE 1024
#define MAC_STRING_LENGTH 13

InfoTCP *Udp::_TcpInfo = ( InfoTCP * ) malloc(sizeof(InfoTCP));
Udp *Udp::_udp;


/// Constructors
Udp::Udp( string interface, int port ): Communication(0x10, 0x22, 0x20 ) {
	_Port = port;
	_Interface = interface;
	_udp = this;
}


void Udp::start(){
	initSocket();
}

/// Init
void Udp::initSocket(){
	/* Create the UDP socket */
	if ((_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		LOGGER_WARN("Udp: Failed to create socket");
		return;
	}


	/* Construct the server sockaddr_in structure */
	memset(&_Server, 0, sizeof(_Server));                 /* Clear struct */
	_Server.sin_family = AF_INET;                            /* Internet/IP */
	_Server.sin_addr.s_addr = inet_addr("255.255.255.255");  /* IP address */
	_Server.sin_port = htons( _Port );                        /* server port */

	int opt = 1;
	setsockopt(_Socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(int));

	pthread_create( &_ThreadListenner , NULL ,  &(this->listen) , (void*) this );
}

void* Udp::listen( void * d){
	Udp* udp = (Udp*) d;

	char buffer[BUFFSIZE];
	unsigned int clientlen = sizeof(udp->_Client);

	memset((void*)&(udp->_Client),0,clientlen);

	LOGGER_VERB("Udp: Wait Data");
	while ( true ){
		int res  = recvfrom( udp->_Socket , buffer, BUFFSIZE, 0, (struct sockaddr *) &(udp->_Client), (socklen_t*) &clientlen);

		if ( res > 0 )
		{
			sprintf(udp->_IpSender,"%s",inet_ntoa(udp->_Client.sin_addr));
			LOGGER_VERB("Get Data From: " << udp->_IpSender) ;
			udp->recieveData( (BYTE*) buffer , res );
		}
	}
	close(udp->_Socket);
	return NULL;
}


void Udp::send ( BYTE* data, int size, char*ipDest ){
	if ( ipDest == NULL || size == 0 )
		return;

	BYTE* datas = NULL;
	int sizeData = encodeData( (BYTE*) data , &datas, size );
	int d = sendto(_udp->_Socket, (char*) datas, sizeData, 0, (struct sockaddr *) &(_udp->_Client), sizeof(_udp->_Client));
	
	//LOGGER_VERB( "UDP Send: " << data[0] ) ;
	
	// Send the word to the server
	if ( d != sizeData) {
		LOGGER_WARN("Mismatch in number of sent bytes: " << sizeData << "!=" << d );
	}
	free(datas);
}

void Udp::sendBroadcast(  ){
	char mac[13];
	mac[0] = (char) GET_MAC_ADDR;
	if( getMac(_Interface.c_str(),mac+1 ) == 0 ){
		LOGGER_WARN( "Interface not found" );
		return;

	}

	BYTE* data = NULL;
	int sizeData = encodeData( (BYTE*) mac , &data, sizeof(mac) );

	if (sendto(_Socket, (char*) data, sizeData, 0, (struct sockaddr *) &_Server, sizeof(_Server)) != sizeData) {
		LOGGER_WARN("Mismatch in number of sent bytes");
		return;
	}
}

InfoTCP* Udp::getInfoSrv(){
	_TcpInfo->port = -1;

	while ( _TcpInfo->port == -1){
		sendBroadcast();
		usleep(1500000);
	}
	if ( _TcpInfo->port == -1 )
		return NULL;

	return _TcpInfo;
}


int getMac(const char *iface, char *dest){
	struct ifreq s;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	strcpy(s.ifr_name, iface);
	if (fd >= 0 && dest && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
	{
		for ( int i = 0; i < 6; ++i){
			snprintf(dest+i*2,MAC_STRING_LENGTH-i*2,"%02x",(unsigned char) s.ifr_addr.sa_data[i]);
		}
		//cout << "Mac addr: " << dest << endl;
	}
	else
	{
		LOGGER_WARN("malloc/socket/ioctl failed");
		close(fd);
		return 0;
	}
	close(fd);
	return 1;
}
