//
//  Recorder.h
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __h__configAppli__
#define __h__configAppli__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

typedef unsigned char BYTE ;
using namespace std;

class ConfigAppli{
	public:
		ConfigAppli ( int port );
		static bool createSocket();	
		static void* run( void* d );
		static void decodeRequest ( char *req );
		
		static void getRecorders( );
		static void getUsersRecorders( );
		static void getUsersWebsite( );
		static void getRooms( );
		static void getCards( );
		
		static void createRooms( string req );
		static void createCards( string req );
		static void createRecorders( string req );
		static void createUsersWebSite( string req );
		static void createUsersRecorder( string req );
		
		static void updateRooms( string req );
		static void updateCards( string req );
		static void updateRecorders( string req );
		static void updateUsersWebSite( string req );
		static void updateUsersRecorder( string req );
	
		static void getImageFromRecorder ( string req );	
		static void parringRecorder ( string req );	
	private:
		static void verifPassword ( string data );
		static bool sendData( string data );
		static char *_DataBuffer;
		static int _Port;
		static int _Socket;
		static int client_sock;
		static bool _Connected;
		static struct sockaddr_in _server , _client;
		static pthread_t _ThreadListenner;
	protected:
};

#endif
