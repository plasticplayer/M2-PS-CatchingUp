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

#include <fstream>


typedef unsigned char BYTE ;
using namespace std;
class Recorder{
	public:
		Recorder ( Udp *udp, char* ip );
		//	~Recorder();
		void getFrameUdp( BYTE* data, unsigned long size );
		void setUdpSocket ( void* sock,  int size );
		static Recorder* findRecorderByIp( char* ip );

		bool isTcpConnected();

		void setTcpSocket( Tcp* s);
		void SRV_TO_REC_askVisuImage( BYTE idCam );
		void SRV_TO_REC_askStatut();
		void SRV_TO_REC_sendParring( );
		char**_Image;
		
		void Parring();
	private:
		static list<Recorder *> _Recorders;


		/// UDP CALLbacks
		
		static void REC_TO_SRV_getMacAddress( BYTE* data, unsigned long size, void *sender);
		static void REC_TO_SRV_ImageInfo( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_imageCompletlySend( BYTE* data, unsigned long size, void *sender );

		/// Tcp Callbacks
		static void REC_TO_SRV_ImagePart( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_recordingEnd( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_getStatut( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_askSendFile( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_ansParring( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_endFileTransfert( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_authentificationAsk( BYTE* data, unsigned long size, void *sender );
		static void REC_TO_SRV_endFilesTransfert( BYTE* data, unsigned long size, void *sender );

	protected:
		bool sendUdpFrame( BYTE* data , int size);
		bool sendTcpFrame( BYTE* data , int size, bool needAck);
		Tcp *_Tcp;
		Communication *_CommunicationUdp;

		Udp *_UdpSrv;
		void* _UdpSocket;
		BYTE _MacAddress[13];
		char *_IpAddr;

		int _Count, _SizeImage;
		//int _TcpSocket;
		int* _ImageParts;
		BYTE _StatutRasp, _StatutArd;

		uint64_t _IdRecorder;
};

#endif /* defined(__server__Recorder__) */
