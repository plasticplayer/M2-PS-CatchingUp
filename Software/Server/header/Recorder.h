//
//  Recorder.h
//  server
//
//  Created by maxime max on 05/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __h__server__Recorder__
#define __h__server__Recorder__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "Communication.h"
#include "Udp.h"
#include "Tcp.h"
#include "hashlibpp.h"
#include "hl_md5wrapper.h"

#include <fstream>


typedef unsigned char BYTE ;

typedef struct UnconnectedClient{
	BYTE _MacAddress[12];
} UnconnectedClient ;

typedef void (*FuncDeco)( void * recorder );

using namespace std;
enum state { WORKING, PASS, FAILED };

class Recorder{
	public:
		Recorder ( Udp *udp, char* ip );
		//	~Recorder();
		bool getImage ( char* image, int* size );
		static void decoTcp( void* recorder );
		void getFrameUdp( BYTE* data, unsigned long size );
		void setUdpSocket ( void* sock,  int size );
		bool isRecording();
		uint16_t filesInWait;
		static Recorder* findRecorderByIp( char* ip );

		bool isTcpConnected();

		void setTcpSocket( Tcp* s);
		void SRV_TO_REC_askStatut();
		void SRV_TO_REC_sendParring( );
		char**_Image;

		char *_IpAddr;
		bool Parring();
		static list<UnconnectedClient*> _UnconnectedClients;
		static Recorder* getRecorderByMac ( BYTE* mac );
		static Recorder* getRecorderById( uint64_t id );

		static void* convertFiles(void * data);
	private:
		static void addUnconnectedClient ( BYTE* mac );
		static void delUnconnectedClient ( BYTE* mac );
		static list<Recorder *> _Recorders;


		static void REC_TO_SRV_TCP_ACK( BYTE* data, unsigned long size, void *sender);
		static void REC_TO_SRV_UDP_ACK( BYTE* data, unsigned long size, void *sender);
		/// UDP CALLbacks
		static void REC_TO_SRV_getMacAddress( BYTE* data, unsigned long size, void *sender);
		static void REC_TO_SRV_ImageInfo( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_imageCompletlySend( BYTE* data, unsigned long size, void *sender );

		/// Tcp Callbacks
		static void REC_TO_SRV_ImagePart( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_createRecording( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_recordingEnd( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_getStatut( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_askSendFile( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_ansParring( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_endFileTransfert( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_authentificationAsk( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_endFilesTransfert( BYTE* data, unsigned long size, void *sender );


		void SRV_TO_REC_askVisuImage( BYTE idCam );
	protected:
		bool sendUdpFrame( BYTE* data , int size);
		bool sendTcpFrame( BYTE* data , int size, bool needAck);
		Tcp *_Tcp;
		Communication *_CommunicationUdp;
		bool _IsRecording;
		Udp *_UdpSrv;
		void* _UdpSocket;
		BYTE _MacAddress[12];

		int _Count, _SizeImage;
		//int _TcpSocket;
		int* _ImageParts;
		BYTE _StatutRasp, _StatutArd;
		state _stateParring, _stateGetImage;
		uint64_t _IdRecorder, _IdUserRecording;
};
#endif /* defined(__server__Recorder__) */
