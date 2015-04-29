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
#include "../header/Config.h"

#define SIZE_BUFFER 128
using namespace std;


/// construtors
Tcp::Tcp( int socket, void* recorder ){
	this->_Recorder = recorder;
	this->_Socket = socket;
	this->_Communication = new Communication( 0x10, 0x22, 0x20, 1024 );
}

/// Listener + sender
void Tcp::start(){
	pthread_create( &_Listenner , NULL ,  &(this->listen) , (void*) this );
	pthread_create( &_Sender , NULL ,  &(this->sends) , (void*) this );
}

int Tcp::getSocket(){
	return _Socket;
}

void* Tcp::listen( void* a){
	Tcp* tcp = (Tcp*) a;
	int read_size;
	char client_message[SIZE_BUFFER];
	
	char tmp[250];
	//Receive a message from client
	while( (read_size = recv( tcp->_Socket , client_message , SIZE_BUFFER , 0)) > 0 )
	{
		memset(tmp,0,sizeof(tmp));
		for(int i = 0; i < ( read_size > 10 ) ? 10 : read_size; i++)
            	sprintf(tmp,"%s %02X",tmp, client_message[i]);
		LOGGER_DEBUG("TCP read" << tmp );
		tcp->_Communication->recieveData((BYTE*)client_message, read_size,tcp->_Recorder );
	}
	LOGGER_DEBUG("TCP : Close socket");
	cout << " " << endl;

	fflush(stdout);
	return NULL;
}

void* Tcp::sends( void* a){
	//Tcp* tcp = (Tcp*) a;
	/*while ( true ) {
		if ( tcp->_Communication->_FrameToSend.size() != 0 ){
			Frame f = tcp->_Communication->_FrameToSend.pop_front();
			tcp->send(f.
		}
	}*/
	return NULL;
}
void Tcp::sendAck( BYTE* Command){
	// TODO: IMPLEMENT
}
void Tcp::sendTcp( BYTE* data, int size , bool needAck){
	// Send Frames
	BYTE* Datas = NULL;
	int res = _Communication->encodeData(data,&Datas,(unsigned long)size);
	if( send( getSocket() , (char*)Datas , res ,0) < 0)
	{
		perror("Send failed : ");
		free(Datas);
		return;
	}
	free(Datas);
}
