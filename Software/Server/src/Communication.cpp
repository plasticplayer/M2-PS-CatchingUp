//
//  Communication.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "../header/Communication.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

Communication::Communication( BYTE start, BYTE stop, BYTE escape, unsigned long buffSize  ){
    this->_Start = start;
    this->_Stop = stop;
    this->_PosDecodeData = 0;
    this->_Escaped = false;
    this->_StartDetected = false;
    this->_Escape = escape;
    this->_Connected = false;
	this->_BuffSize = buffSize;
	
    for (int i = 0; i<256 ; i++) this->_PtrFunctions[i] = NULL;

	this->_DecodeDatas = ( BYTE *) malloc(sizeof(BYTE)*_BuffSize);
    memset(this->_DecodeDatas,0, _BuffSize);
}
/*
~Communication::Communication(){
	free(_DecodeDatas);
	clearCommunications();
}
*/

void Communication::recieveData( BYTE* data, unsigned long size, void* sender ){
    // Set connexion Flag
    this->_Connected = true;
    time(&_LastFrameRecieve);

    for ( unsigned long  i = 0; i < size; i++ ){

        if ( data[i] == this->_Escape ){
            if ( this->_Escaped ){
                this->_DecodeDatas[this->_PosDecodeData++] = this->_Escape;
            }
            this->_Escaped = !this->_Escaped;
        }

        else if ( data[i] == this->_Start ){
            if ( this->_Escaped ){
                this->_DecodeDatas[this->_PosDecodeData++] = this->_Start;
                this->_Escaped = false;
            }
            else{
                // Debut de la trame
                if ( _StartDetected ){
                    // Ancienne trame non complété
                    cout << "Last frame not completed " << endl;
                }
                memset(this->_DecodeDatas,0, _BuffSize);
                this->_PosDecodeData = 0;
                this->_StartDetected = true;
            }
        }

        else if ( data[i] == this->_Stop ){
            if ( this->_Escaped ){
                this->_DecodeDatas[this->_PosDecodeData++] = this->_Stop;
                this->_Escaped = false;
            }
            else{
                if ( _StartDetected ){
                    // Fin de la trame
                    cout << "Get Frame" << endl ;
                    //for ( int i =0; i < this->_PosDecodeData ; i++ )
                    //    printf("%02x ", this->_DecodeDatas[i] );
                    //cout << endl;


                    if ( this->_PosDecodeData > 0 && this->_PtrFunctions[this->_DecodeDatas[0]] != NULL ){
                        unsigned char *recieveData = ( unsigned char * ) malloc ( sizeof( unsigned char) * this->_PosDecodeData -1 );
                        memcpy(recieveData, (this->_DecodeDatas+1), this->_PosDecodeData-1 );
                        this->_PtrFunctions[this->_DecodeDatas[0]](recieveData,this->_PosDecodeData-1,sender);
                        free( recieveData );
                    }
                    else{
                        cout << "No function found: " << this->_DecodeDatas[0]  << endl;
                    }

                    _StartDetected = false;
                }
            }
        }
        else if ( _StartDetected ) {
            if ( this->_Escaped ){
                this->_Escaped = false;
                continue;
            }
            this->_DecodeDatas[this->_PosDecodeData++] = data[i];
        }
    }
}

void Communication::setFunction( FuncType f , int ida){
    _PtrFunctions[ida] = f;
}

int Communication::encodeData( BYTE* dataIn, BYTE** dataOut, unsigned long sizeDataIn ){
    if ( *dataOut != NULL ){
        free ( *dataOut );
    }

    int length = 0, pos = 1 ;
    unsigned long i;
    for ( i = 0; i < sizeDataIn; i++ ){
        if ( dataIn[i] == this->_Start || dataIn[i] == this->_Stop || dataIn[i] == this->_Escape )
            length+=2;
        else
            length++;
    }

    *(dataOut) = ( BYTE* ) malloc(sizeof( BYTE )*(length+3+1));
    (*dataOut)[0] = this->_Start;

    for ( i = 0; i < sizeDataIn; i++ ){
        if ( dataIn[i] == this->_Start || dataIn[i] == this->_Stop || dataIn[i] == this->_Escape ){
            (*dataOut)[pos++] = this->_Escape;
        }
        (*dataOut)[pos++] = dataIn[i];
    }
    (*dataOut)[pos++] = this->_Stop;
    (*dataOut)[pos] = '\0';

    return pos;
}

void Communication::sendAck( HEADER_Protocol head ){
    Frame *f = ( Frame *) malloc(sizeof(Frame));
    f->header = ACK;
    f->needAck = false;
    f->data = NULL;
    BYTE data[] = {ACK, (BYTE)head };
    f->size = encodeData(data ,&(f->data),2);
    f->lastSend = 0;
    f->maxRetry = 0;
    _AckToSend.push_back(*f);
}

void Communication::sendBuffer( HEADER_Protocol header, BYTE* data, unsigned long length, bool needAck, int nretry ){
    Frame *f = ( Frame *) malloc(sizeof(Frame));
    f->header = header;
    f->needAck = needAck;
    f->data = NULL;
    f->size = encodeData(data,&(f->data),length);
    f->maxRetry = nretry;
    f->lastSend = 0;
    _FrameToSend.push_back(*f);
}

void Communication::clearCommunications(){
    while ( !_FrameToSend.empty() ){
        Frame f = _FrameToSend.front();
        _FrameToSend.pop_front();
        free(f.data);
    }
     while ( !_AckToSend.empty() ){
        Frame f = _AckToSend.front();
        _AckToSend.pop_front();
        free(f.data);
    }
}
