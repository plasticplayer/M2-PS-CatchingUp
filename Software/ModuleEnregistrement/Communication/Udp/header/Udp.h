//
//  Udp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Udp__
#define __server__Udp__

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "Communication.h"



typedef unsigned char BYTE ;


class Udp : Communication {
	public:
		Udp( int port );
		Udp( );

		void sendImage();
		InfoTCP* getInfoSrv( int port );

	protected:
	private:
		static Udp *_udp;
		static void ansInfoSrv ( BYTE data[], int size );
		static void recieveAckImage ( BYTE data[], int size );
		static void* listen(void *data);

		void initSocket();
		void sendBroadcast();
		void sendAckImage();
		void send ( BYTE* data, int size, char*ipDest );
		void sendImagePart( int noPart );
		int _Port, _Socket ;
		char _IpSender[20];
		pthread_t _ThreadListenner;
		char* _ImageContent; 
		int _ImageParts, _ImageSize;
		struct sockaddr_in _Server,_Client;
		bool _WaitAckImage;
};
int getMac( char* iface, char* dest);
#endif /* defined(__server__Udp__) */
