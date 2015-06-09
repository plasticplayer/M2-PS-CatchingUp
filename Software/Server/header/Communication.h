//
//  Communication.h
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __ProtocolCommunication__Communication__
#define __ProtocolCommunication__Communication__

typedef unsigned char BYTE ;

#include <stdio.h>
#include <ctime>
#include <time.h>
#include <iostream>
#include <string>
#include <list>

#define TIME_RETRY 2
#define TIME_POOLING 20
#define TIME_MAX_APPAIRAGE_ S   30

#define ADDR_PARKING_TX 0xF2000000LL
#define ADDR_PARKING_RX 0xF1000000LL

using namespace std;



enum HEADER_Protocol {
	//// NRF HEADERS
	
	/// Authentification
	AUTH_ASK = 0x05,
	AUTH_ANS = 0x06,

	/// Pooling ADDR
	POOL_PARK_ADDR = 0x01,
	POOL_PARK_ADDR_ANS = 0x02,
	POOL_SET_ADDR = 0x03,
	POOL_VALIDATE_CONFIG = 0x04,

	/// Control
	ACTIVATE_RECORDING = 0x07,

	/// Status
	STATUS_ASK = 0x0A,
	STATUS_ANS = 0x0B,

	//// TCP HEADERS
	SEND_ASK_PAIRING = 0x21,
	ANS_ASK_APPARAIGE = 0x22,
	GET_AUTHENTIFICATION_REQ = 0x23,
	ANS_AUTHENTIFICATION = 0x24,
	SEND_ASK_STATUT = 0x25,
	GET_STATUT = 0x26,
	ASK_STORAGE = 0x27,
	ANS_STORAGE = 0x28,
	GET_END_FILE_TRANSFERT = 0x29,
	ANS_END_FILE_TRANSFERT = 0x2A,
	GET_END_TRANSFERT_FILES = 0x2B,
	ASK_VISU_CAM = 0x2C,
	GET_RECORDING_START = 0x2D,
	SEND_RECORDING_ID = 0x2E,
	GET_RecordingEnd = 0x30,
	ACK_TCP = 0x2F,
	
	//// UDP HEADERS
	
	/// ACK
	ACK = 0x0F,

	GET_MAC_ADDR = 0x41,
	GET_IMAGE_INFO = 0x43,
	GET_IMAGE_PARTS = 0x45,
	GET_IMAGE_COMPLETLY_SEND = 0x46,
	SEND_PARTS_NOT_RECIEVED = 0x47,
	ACK_UDP = 0x4F
};



typedef void (*FuncType)( unsigned char data[], unsigned long size, void* sender);

typedef struct { HEADER_Protocol header; BYTE *data; unsigned long size; bool needAck; time_t lastSend; int maxRetry;  }Frame;


class Communication {

	public:
		Communication( BYTE start, BYTE stop, BYTE escape, unsigned long buffSize );
		//~Communication();
		void recieveData( BYTE* data, unsigned long size, void* sender );
		void clearCommunications();

		void sendAck( HEADER_Protocol header );
		int encodeData( BYTE* dataIn, BYTE** dataOut, unsigned long sizeDataIn );
		void setFunction( FuncType f, int ida);
		void sendBuffer( HEADER_Protocol header, BYTE* data, unsigned long length, bool needAck, int Retry );
		list<Frame> _FrameToSend;
		list<Frame> _AckToSend;

	protected:
		bool _Connected;
		unsigned long _BuffSize;
		bool _Escaped, _StartDetected;
		FuncType _PtrFunctions[256] ;
		BYTE *_DecodeDatas, _Start, _Stop, _Escape;
		unsigned long _PosDecodeData;
		time_t _LastFrameRecieve;
};


#endif /* defined(__ProtocolCommunication__Communication__) */
