//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Config.h"
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
#define MAC_STRING_LENGTH 13

InfoTCP * Udp::_TcpInfo;
Udp * Udp::_udp;


void Udp::ansInfoSrv ( BYTE data[], int size )
{
	if ( size != 2 )
		return;

	_TcpInfo->port = data[0]<<8 | data[1] ;

	//BYTE *ans = ( BYTE*) malloc( sizeof( BYTE ) * 2);
	BYTE ans[2];
	ans[0] = ACK_UDP;
	ans[1] = GET_INFO_SRV;


	BYTE* datas = NULL;

	//for (int i = 0; i < size ; i++ )
	//{
	//	printf(" %02x", data[i]);
		//fflush(stdout);
	//}

	int sizeData = _udp->encodeData( data , &datas, sizeof( data ) );



	//_udp->send( ans, 2 , _TcpInfo.ipAddress);
}

void Udp::send ( BYTE* data, int size, char*ipDest )
{

	/* Create the UDP socket */
	int sock;

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		LOGGER_ERROR("Failed to create UDP socket");
		return;
	}

	struct sockaddr_in echoserver;
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));                 /* Clear struct */
	echoserver.sin_family = AF_INET;                            /* Internet/IP  */
	echoserver.sin_addr.s_addr = inet_addr( ipDest );  			/* IP address   */


	echoserver.sin_port = htons( 1902 );                        /* server port  */

	BYTE* datas = NULL;

	for (int i = 0; i < size ; i++ )
	{
		printf(" %02x", data[i]);
		fflush(stdout);
	}

	int sizeData = encodeData( (BYTE*) data , &datas, size );

	/*// Send the word to the server
	if (sendto(sock, (char*) datas, sizeData, 0, (struct sockaddr *) &echoserver, sizeof(echoserver)) != sizeData) {
		cout << "Mismatch in number of sent bytes" << endl;
	}

	free(datas);
	close(sock);*/
}


int getMac(const char *iface, char *dest)
{
	struct ifreq s;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	strcpy(s.ifr_name, iface);
	if (fd >= 0 && dest && 0 == ioctl(fd, SIOCGIFHWADDR, &s))
	{
		for ( int i = 0; i < 6; ++i)
		{
			//printf( "%02x ", s.ifr_addr.sa_data[i] );
			snprintf(dest+i*2,MAC_STRING_LENGTH-i*2,"%02X",(unsigned char) s.ifr_addr.sa_data[i]);
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

Udp::Udp( string iface , int port ) : Communication(0x10, 0x22, 0x20 )
{
	_Port = port;
	_interface = iface;
	setFunction( (FuncType) &ansInfoSrv, (int)GET_INFO_SRV );
    _TcpInfo = new InfoTCP();
    _udp = this;
}
Udp::Udp( int port ) : Communication(0x10, 0x22, 0x20 )
{
	_Port = port;
	_interface = "eth0";
	setFunction( (FuncType) &ansInfoSrv, (int)GET_INFO_SRV );
    _TcpInfo = new InfoTCP();
    _udp = this;
}
Udp::Udp( ) : Communication(0x10, 0x22, 0x20 )
{
	_Port = 1902;
	_interface = "eth0";
	setFunction( (FuncType) &ansInfoSrv , (int)GET_INFO_SRV );
    _TcpInfo = new InfoTCP();
    _udp = this;
}




InfoTCP* Udp::getInfoSrv()
{
	// Declare TimeOut
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 000;


	char mac[13];
	// Get Mac ADDRESS

	mac[0] = (char)GET_MAC_ADDR;
	if( getMac(_interface.c_str(),mac+1 ) == 0 )
	{
		LOGGER_ERROR("Interface " << _interface << " not found");
		return NULL;
	}

    LOGGER_INFO("Interface " << _interface << " has mac : "<< (mac+1));
	BYTE* data = NULL;

	int sizeData = encodeData( (BYTE*) mac , &data, sizeof(mac) );


	int sock;
	char buffer[BUFFSIZE];
	unsigned int clientlen;
	struct sockaddr_in echoserver, echoclient;


	memset((void*)&echoclient,0,sizeof(echoclient));


	/* Create the UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		LOGGER_ERROR("Failed to create socket");
		return NULL;
	}


	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));                 /* Clear struct */
	echoserver.sin_family = AF_INET;                            /* Internet/IP */
	echoserver.sin_addr.s_addr = inet_addr("255.255.255.255");  /* IP address */
	echoserver.sin_port = htons( _Port );                        /* server port */


	/* Set socket option to broadcast*/
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(int));

	int ret = 0;
	if (( ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) ) < 0)
	{
		LOGGER_WARN("Error affecting Time Out : " << ret);
	}

	_TcpInfo->port = -1;

	// Renvoie 10 fois la requetes si pas de réponse
	for ( int i = 0; i < 10; i++ )
	{
		if ( _TcpInfo->port != -1)
			break;

		LOGGER_VERB("Sending UDP Brodcast");

		/* Send the word to the server */
		if (sendto(sock, (char*) data, sizeData, 0, (struct sockaddr *) &echoserver, sizeof(echoserver)) != sizeData)
		{
			LOGGER_WARN("Mismatch in number of sent bytes");
			continue;
		}

		clientlen = sizeof(echoclient);

		int res  = recvfrom(sock, buffer, BUFFSIZE, 0, (struct sockaddr *) &echoclient, (socklen_t*) &clientlen);

		if ( res > 0 )
		{
			LOGGER_VERB("Get Data From: " << inet_ntoa(echoclient.sin_addr));
			recieveData( (BYTE*) buffer , res );
			strcpy(_TcpInfo->ipAddress, inet_ntoa(echoclient.sin_addr)) ;
		}
	}

	close(sock);
	if ( _TcpInfo->port == -1 )
		return NULL;

	return _TcpInfo;
}


