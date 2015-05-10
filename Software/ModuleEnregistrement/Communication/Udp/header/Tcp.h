//
//  Tcp.h
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __server__Tcp__
#define __server__Tcp__

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#include "Communication.h"



typedef unsigned char BYTE ;


class Tcp : Communication {
public:
   	Tcp( InfoTCP* infoConnexion );
   	
	void start();
	
protected:


private:
	static Tcp *_tcp;
	int _Sock;
	struct sockaddr_in _Server;
};

#endif /* defined(__server__Tcp__) */
