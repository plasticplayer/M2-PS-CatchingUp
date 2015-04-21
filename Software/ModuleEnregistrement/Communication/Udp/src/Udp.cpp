//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

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

InfoTCP _TcpInfo;
Udp *Udp::_udp;


/// Constructors
Udp::Udp( int port ) : Communication(0x10, 0x22, 0x20 ) {
	_Port = port;
	_ImageContent = NULL;
	_udp = this;
	initSocket();
}

Udp::Udp( ) : Communication(0x10, 0x22, 0x20 ) {
	_Port = 1902;
	_ImageContent = NULL;
	_udp = this;
	initSocket();
}


/// Init
void Udp::initSocket(){
	setFunction( (FuncType) &ansInfoSrv, (int)GET_INFO_SRV );
	setFunction( (FuncType) &recieveAckImage, (int) GET_IMAGE_ACK );

	/* Create the UDP socket */
	if ((_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Failed to create socket");
		return;
	}


	/* Construct the server sockaddr_in structure */
	memset(&_Server, 0, sizeof(_Server));                 /* Clear struct */
	_Server.sin_family = AF_INET;                            /* Internet/IP */
	_Server.sin_addr.s_addr = inet_addr("255.255.255.255");  /* IP address */
	_Server.sin_port = htons( _Port );                        /* server port */
	
	int opt = 1;
	setsockopt(_Socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(int));
	
	if( pthread_create( &_ThreadListenner , NULL ,  &(this->listen) , (void*) this ) < 0)
    {
         cout << " Error: could not create thread" << endl;
    }
}

void* Udp::listen( void * d){
	Udp* udp = (Udp*) d;
	
	char buffer[BUFFSIZE];
	unsigned int clientlen = sizeof(udp->_Client);
	//struct sockaddr_in echoclient;

	memset((void*)&(udp->_Client),0,clientlen);

	cout << "Wait UDP data" << endl;
	while ( true ){
		int res  = recvfrom( udp->_Socket , buffer, BUFFSIZE, 0, (struct sockaddr *) &(udp->_Client), (socklen_t*) &clientlen);

		if ( res > 0 )
		{
			sprintf(udp->_IpSender,"%s",inet_ntoa(udp->_Client.sin_addr));
			cout << "Get Data From: " << udp->_IpSender ;
			for ( int i = 0; i < res ; i++ ) printf(" %02x",buffer[i]);
			fflush(stdout);
			udp->recieveData( (BYTE*) buffer , res );
		}
	}
	close(udp->_Socket);
	return NULL;
}
	

/// CallBacks
void Udp::ansInfoSrv ( BYTE data[], int size ){
	if ( size != 2 )
		return;

	_TcpInfo.port = data[0]<<8 | data[1] ;
	sprintf( _TcpInfo.ipAddress ,"%s",_udp->_IpSender);
	BYTE ans[2];
	ans[0] = ACK_UDP;
	ans[1] = GET_INFO_SRV;

	BYTE* datas = NULL; 
	int sizeData = _udp->encodeData( ans , &datas, 2 );

	_udp->send(  datas, sizeData , _TcpInfo.ipAddress);
	free(datas);
}

void Udp::sendAckImage(){
	BYTE datas[ 3 ];
	datas[0] = SEND_IMAGE_COMPLETLY_SEND;
	datas[1] = (_ImageParts >> 8)&0xFF;
	datas[2] = _ImageParts & 0xFF;
	_WaitAckImage = true;
	
	//while ( _WaitAckImage )
	{
		usleep(100000);
		send(datas,3,_TcpInfo.ipAddress);
	}
}

void Udp::recieveAckImage ( BYTE data[], int size ){
	_udp->_WaitAckImage = false;
	cout << "Ack Image: ";
	fflush(stdout);
	cout << "Resend " << size/2 << " Parts" << endl;
	for ( int i = 0 ; i < size; i+=2 ){
		_udp->sendImagePart(data[i]<<8 | data[i+1]  );
	}
	if ( size != 0 ){
		_udp->sendAckImage();
	}
	else{
		// Image completly recied
		cout << "Image completly recieved" << endl;
		free( _udp->_ImageContent );
		_udp->_ImageContent = NULL;
	}
}


/// Send Data
void Udp::sendImage(){

	if ( _TcpInfo.port == -1 ){
		return;
	}

	streampos size;

	ifstream file ("img.jpg", ios::in|ios::binary|ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		if ( _ImageContent != NULL ) 
			free(_ImageContent);

		_ImageContent = new char [size];
		_ImageSize = size;
		file.seekg (0, ios::beg);
		file.read (_ImageContent, size);
		file.close();

		cout << "Image load" << endl;

		_ImageParts = (size)/(MAX_SIZE_UDP);
		if ( (size % MAX_SIZE_UDP) != 0) _ImageParts++;



		cout << "Number of parts: " << _ImageParts << endl;
		// Send image size


		BYTE datas[ 3 ];
		datas[0] = SEND_IMAGE_INFO;
		datas[1] = (_ImageParts >> 8)&0xFF;
		datas[2] = _ImageParts & 0xFF;
		send(datas,3,_TcpInfo.ipAddress);	

		datas[0] = SEND_IMAGE;

		for (int i = 0; i < _ImageParts; i++){
			sendImagePart( i );
		}
		_udp->sendAckImage();
	}
	else cout << "Unable to open file" << endl;
}

void Udp::sendImagePart( int noPart ){
	BYTE datas[ MAX_SIZE_UDP+3 ];
	datas[0] = SEND_IMAGE;

	datas[1] = (noPart >> 8)&0xFF;
	datas[2] = noPart & 0xFF;

	if ( noPart != ( _ImageParts-1) ){
		memcpy(datas+3,_ImageContent+(noPart*MAX_SIZE_UDP),MAX_SIZE_UDP);
		send(datas,MAX_SIZE_UDP+3,_TcpInfo.ipAddress);	
	}
	else{
		memset(datas+3,0,MAX_SIZE_UDP);
		int s = _udp->_ImageSize - (noPart*MAX_SIZE_UDP);
		memcpy(datas+3,_ImageContent+(noPart*MAX_SIZE_UDP),s);
		send(datas,s+3,_TcpInfo.ipAddress);	
	}
	usleep(15);
}


void Udp::send ( BYTE* data, int size, char*ipDest ){
	if ( ipDest == NULL || size == 0 )
		return;

	BYTE* datas = NULL;
	int sizeData = encodeData( (BYTE*) data , &datas, size );
	int d = sendto(_udp->_Socket, (char*) datas, sizeData, 0, (struct sockaddr *) &(_udp->_Client), sizeof(_udp->_Client));
	
	// Send the word to the server
	if ( d != sizeData) {
	  cout << "Mismatch in number of sent bytes: " << sizeData << "!=" << d << endl;
	}
	free(datas);
	//close(sock);
}

void Udp::sendBroadcast(  ){
	char mac[13];
	mac[0] = (char) GET_MAC_ADDR;
	if( getMac((char *)"eth0",mac+1 ) == 0 ){
		cout << "Interface not found" << endl;
		return;
	}

	BYTE* data = NULL;
	int sizeData = encodeData( (BYTE*) mac , &data, sizeof(mac) );
	
	if (sendto(_Socket, (char*) data, sizeData, 0, (struct sockaddr *) &_Server, sizeof(_Server)) != sizeData) {
		printf("Mismatch in number of sent bytes");
		return;
	}
}

InfoTCP* Udp::getInfoSrv( int port ){
	_TcpInfo.port = -1;
	
	while ( _TcpInfo.port == -1){
		cout << "Send Brodcast" << endl;

		sendBroadcast();
		usleep(1500000);
	}
	if ( _TcpInfo.port == -1 )
		return NULL;

	return &_TcpInfo;
}


int getMac(char *iface, char *dest){
struct ifreq s;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	strcpy(s.ifr_name, iface);
	if (fd >= 0 && dest && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
	{
		for ( int i = 0; i < 6; ++i){
			//printf( "%02x ", s.ifr_addr.sa_data[i] );
			snprintf(dest+i*2,MAC_STRING_LENGTH-i*2,"%02x",(unsigned char) s.ifr_addr.sa_data[i]);
		}
		//cout << "Mac addr: " << dest << endl;
	}
	else
	{
		perror("malloc/socket/ioctl failed");
		close(fd);
		return 0;
	}
	close(fd);
	return 1;
}
