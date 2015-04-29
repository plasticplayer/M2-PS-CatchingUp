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

#define BUFFER_SIZE 1024

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
#include "logger.h"


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

	/// ACK
	ACK = 0x0F,

	//// TCP HEADERS
	ASK_PARIRING = 0x21,
	ANS_PARIRING = 0x22,
	SEND_AUTHENTIICATION_REQUEST_TCP = 0x23,
	GET_AUTHENTIFICATION_ANS_TCP = 0x24,
	ASK_STATUS_TCP = 0x25,
	ANS_STATUS_TCP = 0x26,
	SEND_STORAGE_REQUEST = 0x27,
	GET_STORAGE_ANS = 0x28,
	SEND_FILE_SEND_TCP = 0x29,
	ACK_FILE_SEND_TCP = 0x2A,
	ALL_FILES_SEND = 0x2B,
	ASK_VISU_CAM = 0x2C,
	ASK_ID_RECORDING = 0x2D,
	ANS_ID_RECORDING = 0x2E,
	SEND_RecordingEnd = 0x2F,
	ACK_TCP = 0x2F,
	
	//// UDP HEADERS
	GET_MAC_ADDR = 0x41,
	GET_INFO_SRV = 0x42,
	SEND_IMAGE_INFO = 0x43,
	SEND_IMAGE = 0x45,
	SEND_IMAGE_COMPLETLY_SEND = 0x46,
	GET_IMAGE_ACK = 0x47,
	ACK_UDP = 0x4F
};



typedef void (*FuncType)( unsigned char data[], int size);

typedef struct { HEADER_Protocol header; BYTE *data; int size; bool needAck; time_t lastSend; int maxRetry;  } Frame;
typedef struct InfoTCP{ char ipAddress[20]; int port;} InfoTCP;

class Communication {

	public:
		Communication( BYTE start, BYTE stop, BYTE escape );
		void recieveData( BYTE* data, int size );
		void clearCommunications();

		void sendAck( HEADER_Protocol header );
		int encodeData( BYTE* dataIn, BYTE** dataOut, int sizeDataIn );
		void setFunction( FuncType f , int ida);
		void sendBuffer( HEADER_Protocol header, BYTE* data, int length, bool needAck, int Retry );
		list<Frame> _FrameToSend;
		list<Frame> _AckToSend;
	protected:
		bool _Connected;

		bool _Escaped, _StartDetected;
		FuncType _PtrFunctions[256] ;
		BYTE _DecodeDatas[1024], _Start, _Stop, _Escape;
		int _PosDecodeData;
		time_t _LastFrameRecieve;
};


#endif /* defined(__ProtocolCommunication__Communication__) */
