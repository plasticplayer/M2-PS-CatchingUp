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
#include "logger.h"


typedef unsigned char BYTE ;


class Udp : public Communication {
	public:
		Udp( string interface, int port );
		void start();
		void send( BYTE* data, int size, char*ipDest );
		static InfoTCP *_TcpInfo;
		void sendImage();
		InfoTCP* getInfoSrv( );
		static Udp*_udp;
		char _IpSender[20];
	protected:
	private:
		static void ansInfoSrv ( BYTE data[], int size );
		static void recieveAckImage ( BYTE data[], int size );
		static void* listen(void *data);

		void initSocket();
		void sendBroadcast();
		int _Port, _Socket ;
		string _Interface;
		pthread_t _ThreadListenner;
		struct sockaddr_in _Server,_Client;
};
int getMac( const char* iface, char* dest);
#endif /* defined(__server__Udp__) */
