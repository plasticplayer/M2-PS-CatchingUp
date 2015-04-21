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

#include "../header/Tcp.h"
#define SIZE_BUFFER 128
using namespace std;

Tcp::Tcp( int socket ){
	this->_Socket = socket;
	this->_Communication = new Communication( 0x10, 0x22, 0x20, 1024 ); 

}


void Tcp::start(){
	if( pthread_create( &_Listenner , NULL ,  &(this->listen) , (void*) this ) < 0)
	{
		cout << " Error: could not create thread" << endl;
	}
}

void* Tcp::listen( void* a){
	Tcp* tcp = (Tcp*) a;
	int read_size;
	char client_message[SIZE_BUFFER];

	//Receive a message from client
	while( (read_size = recv( tcp->_Socket , client_message , SIZE_BUFFER , 0)) > 0 )
	{
		cout << "Tcp: " << read_size << endl;
		//	_Communication->recieveData(client_message, read_size, (void*) this);
	}

	// _Recorder->setTcpSocket( -1 );

	cout << "Close socket " << endl;

	fflush(stdout);
	return NULL;
}


void send(){
	// Send Frames
}
