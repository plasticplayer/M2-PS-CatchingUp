//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "../header/Tcp_Socket_Server.h"
#include "../header/Tcp.h"

#include <unistd.h>
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


#define BUFFSIZE 256


using namespace std;




Tcp_Socket_Server* Tcp_Socket_Server::tcp_socket_server = NULL ;
int Tcp_Socket_Server::_Port ;

Tcp_Socket_Server::Tcp_Socket_Server( int port ) {
    _Port = port;
    tcp_socket_server = this;
}

int Tcp_Socket_Server::getPort(){
	if ( tcp_socket_server == NULL )
		return -1;
		
	else return _Port;
}

void Tcp_Socket_Server::start(){

	if( pthread_create( &_ThreadListenner , NULL ,  &(this->listenner) , (void*) this ) < 0)
    {
         cout << " Error: could not create thread" << endl;
     }
}




void* Tcp_Socket_Server::listenner( void* data){

	Tcp* tcp = (Tcp*) data;
	
	int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    
    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( _Port );
    
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return NULL;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Tcp:: Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    
    Recorder *r;
	
    while( ( client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        
		r = Recorder::findRecorderByIp( inet_ntoa(client.sin_addr) );
		
		if ( r == NULL ){
			continue;
			//cout << "New recorder " << endl;
			//fflush( stdout );
            		//r = new Recorder( NULL, ipClient);
        }

		Tcp *client = new Tcp(  client_sock );
//        pthread_t t;
        r->setTcpSocket(client);
        //tcp->_RecordersThreads.push_front(t);
        
        
  /*      if( pthread_create( &t , NULL ,  &(client->listenner) , NULL) < 0)
        {
            perror("could not create thread");
            return NULL;
        }*/
    }
    return NULL;
        
}
