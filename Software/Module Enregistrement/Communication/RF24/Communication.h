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

#define TIME_RETRY_S 2

using namespace std;

enum HEADER_Protocol {
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
    STAUTS_ANS = 0x0B,

    /// ACK
    ACK = 0x0F
};
enum StateAutomate { WAIT_FOR_DATA, WAIT_ACK, POOLING, STAND_BY };


typedef void (*FuncType)( unsigned char data[], int size);

typedef struct { HEADER_Protocol header; BYTE *data; int size; bool needAck; time_t lastSend; }Frame;




class Communication {

public:

    void sendAck( HEADER_Protocol header );
    int encodeData( BYTE* dataIn, BYTE** dataOut, int sizeDataIn );
    void setFunction( void (*FuncType)( BYTE data[], int size) , int ida);
    void sendBuffer( HEADER_Protocol header, BYTE* data, int length, bool needAck );

protected:
    Communication( BYTE start, BYTE stop, BYTE escape );
    bool _Escaped, _StartDetected;
    FuncType _PtrFunctions[256];
    BYTE _DecodeDatas[1024], _Start, _Stop, _Escape;
    int _PosDecodeData;


    void recieveData( BYTE* data, int size );


    list<Frame> _FrameToSend;
    StateAutomate state;

};


#endif /* defined(__ProtocolCommunication__Communication__) */
