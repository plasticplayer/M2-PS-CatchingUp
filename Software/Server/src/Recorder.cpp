//
//  Recorder.cpp
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#define IMAGE_SIZE_MAX 2000
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "../header/define.h"
#include "../header/Recorder.h"
#include "../header/Communication.h"

list<Recorder *> Recorder::_Recorders;
using namespace std;


Recorder::Recorder ( Udp *udp, char* ip ){
	this->_UdpSrv = udp;
	this->_UdpSocket = NULL;
	this->_ImageParts = NULL;
	this->_Tcp = NULL;

	this->_IpAddr = ( char *) malloc ( strlen(ip) );
	memcpy( _IpAddr,ip,strlen(ip) );

	// Configure Udp callbacks
	this->_CommunicationUdp = new Communication( 0x10, 0x22, 0x20, IMAGE_SIZE_MAX ); 
	_CommunicationUdp->setFunction((FuncType)&(this->getMacAddress), (int)GET_MAC_ADDR);
	_CommunicationUdp->setFunction((FuncType)&(this->getImageParts), (int)GET_IMAGE_PARTS);
	_CommunicationUdp->setFunction((FuncType)&(this->getImageInfo), (int)GET_IMAGE_INFO);

	_CommunicationUdp->setFunction((FuncType)&(this->imageCompletlySend), (int)GET_IMAGE_COMPLETLY_SEND);


	// Add recorder to List
	_Recorders.push_back(this);
}

/*
   ~Recorder::Recorder(){
   _Recorders.remove(this);
   delete this->_CommunicationUdp;
   this->_CommunicationUdp = NULL;
   free(this->_IpAddr);

   }
   */



bool Recorder::isTcpConnected(){
	return ( this->_Tcp != NULL );
}

void Recorder::setTcpSocket( Tcp* s ){
	this->_Tcp = s;
}



void Recorder::getMacAddress( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;

	cout << "Get Mac Addr: ";


	memcpy( rec->_MacAddress, data , size );
	for (unsigned long i = 0; i < size; i++ ) printf("%c", rec->_MacAddress[i] );


	cout << " On Ip: " << rec->_IpAddr << endl;

	fflush(stdout);

	int portTcp = TCP_PORT;
	if ( portTcp == -1 )
		return;

	BYTE req[] = { 0x42 , (BYTE)(portTcp >> 8), (BYTE)(portTcp & 0xFF) };
	BYTE* ans = NULL;

	int t = rec->_CommunicationUdp->encodeData(req,&ans, sizeof(req));
	rec->sendUdpFrame(ans, t);
}

void Recorder::getImageInfo( BYTE* data, unsigned long size, void *sender ){
	if ( size != 2 )
		return;

	int ss = data[0] <<8 | data[1];
	cout << "Get Image info. nbParts: " << "Data: " << ss << endl;

	Recorder *rec = (Recorder*) sender;

	
	if ( rec->_Image != NULL ){
		//cout << "Free :" << rec->_SizeImage << endl;
		for (int i =0; i < rec->_SizeImage ;i++){
			if ( rec->_Image[i] != NULL ){
				free(rec->_Image[i]);
				rec->_Image[i] = NULL;
			}
		}
		cout << "Free 0" << endl;
		fflush(stdout);
		free(rec->_Image);
		rec->_Image = NULL;
	}
	cout << "Free 1" << endl;
	fflush(stdout);
	
	if ( rec->_ImageParts != NULL ){
		free ( rec->_ImageParts );
		rec->_ImageParts = NULL;
	}
		
	
	rec->_ImageParts = ( int * ) malloc ( sizeof(int) * ss);
	

	memset(rec->_ImageParts,0,ss);
	
	

	rec->_Image = ( char**) malloc ( sizeof(char*) * ss );
	memset(rec->_Image,0,sizeof(char*) * ss);
	rec->_SizeImage = ss;
	rec->_Count = 0;
}

void Recorder::getImageParts( BYTE* data, unsigned long size, void *sender ){

	if ( size < 3 )
		return;

	Recorder *rec = (Recorder*) sender;
	if ( rec->_Image == NULL ) return;

	int partNo = data[0]<<8 | data[1];
	rec->_ImageParts[ partNo ] = size - 2;

	rec->_Count++;
	//	cout << "Get Image Parts: " << partNo << " ==> " << rec->_Count << endl;


	if ( rec->_Image[partNo] == NULL ){
		rec->_Image[partNo] = ( char* ) malloc (sizeof(char)*size-2);
		memcpy(rec->_Image[partNo],data+2,size-2);
	}
}

void Recorder::imageCompletlySend( BYTE* data, unsigned long size, void *sender ){
	Recorder *rec = (Recorder*) sender;
	list<int> l;

	cout << "Get Completly " << endl;
	for (int i = 0;  i < rec->_SizeImage ;i++){
		if ( rec->_ImageParts[i] == 0 ){
			l.push_back(i);
		}
		if ( l.size() > 150 )
			break;
	}
	int siz = 1 + l.size() * 2 ; 
	BYTE* req = new BYTE[ 1 + l.size() * 2];
	req[0] = SEND_PARTS_NOT_RECIEVED ;
	int i = 1;


	if ( l.size() == 0 ) {
		// Y a toute l'image
		ofstream myFile ("img.jpg", ios::out | ios::binary);
		for (int i =0; i < rec->_SizeImage ;i++){
			myFile.write (rec->_Image[i], rec->_ImageParts[ i ] );
		}
		myFile.close();
	}
	else
		cout << "manque : " << l.size() << endl;
	while ( l.size() != 0 ){
		int d = l.front();
		l.pop_front();
		req[i++] = d >> 8 & 0xFF;
		req[i++] = d & 0xFF;
		//		cout << "Request for " << d << endl;
	}

	// Send Data
	BYTE* ans = NULL;
	int t = rec->_CommunicationUdp->encodeData(req, &ans, siz );
	usleep(100000);
	rec->sendUdpFrame(ans, t);
}


void Recorder::getFrameUdp( BYTE* data, unsigned long size  ){
	if ( _CommunicationUdp == NULL )
		return;
	//for (unsigned long i = 0; i < size; i++ ) printf("%02x ", data[i] );
	_CommunicationUdp->recieveData(data, size, (void*) this);
}

void Recorder::setUdpSocket ( void* sock, int size ){
	if ( this->_UdpSocket != NULL )
		free ( this->_UdpSocket );

	this->_UdpSocket = malloc( size );
	memcpy( this->_UdpSocket , sock, size );

}

bool Recorder::sendUdpFrame( BYTE* data, int size ){
	return this->_UdpSrv->sendFrame(_UdpSocket, data, size);
}

Recorder* Recorder::findRecorderByIp( char* ip ){
	if ( _Recorders.empty() )
		return NULL;

	for ( list<Recorder*>::iterator it = _Recorders.begin(); it != _Recorders.end(); ++it ){
		Recorder *rec = *it;
		if ( strcmp(ip,rec->_IpAddr) == 0 )
			return rec;
	}
	return NULL;
}
