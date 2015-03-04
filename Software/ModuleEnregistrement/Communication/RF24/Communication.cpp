//
//  Communication.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Communication.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

Communication::Communication( BYTE start, BYTE stop, BYTE escape  ){
    this->_Start = start;
    this->_Stop = stop;
    this->_PosDecodeData = 0;
    this->_Escaped = false;
    this->_StartDetected = false;
    this->_Escape = escape;
    this->_Connected = false;

    for (int i = 0; i<256 ; i++) this->_PtrFunctions[i] = NULL;

    memset(this->_DecodeDatas,0, BUFFER_SIZE);
}

void Communication::recieveData( BYTE* data, int size ){
    // Set connexion Flag
    this->_Connected = true;
    time(&_LastFrameRecieve);

    for (int i = 0; i < size; i++ ){

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
                memset(this->_DecodeDatas,0, BUFFER_SIZE);
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
                    cout << "Get Frame: " ;
                    for ( int i =0; i < this->_PosDecodeData ; i++ )
                        printf("%02x ", this->_DecodeDatas[i] );
                    cout << endl;


                    if ( this->_PosDecodeData > 0 && this->_PtrFunctions[this->_DecodeDatas[0]] != NULL ){
                        unsigned char *recieveData = ( unsigned char * ) malloc ( sizeof( unsigned char) * this->_PosDecodeData -1 );
                        memcpy(recieveData, (this->_DecodeDatas+1), this->_PosDecodeData-1 );
                        this->_PtrFunctions[this->_DecodeDatas[0]](recieveData,this->_PosDecodeData-1);
                        free( recieveData );
                    }
                    else{
                        cout << "No function found" << endl;
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

void Communication::setFunction( void (*FuncType)( BYTE data[], int size) , int ida){
    _PtrFunctions[ida] = FuncType;
}

int Communication::encodeData( BYTE* dataIn, BYTE** dataOut, int sizeDataIn ){
    if ( *dataOut != NULL ){
        free ( *dataOut );
    }

    int length = 0, i, pos = 1 ;

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

void Communication::sendAck( HEADER_Protocol header ){
    Frame *f = ( Frame *) malloc(sizeof(Frame));
    f->header = ACK;
    f->needAck = false;
    f->data = NULL;
    BYTE data[] = {ACK,header};
    f->size = encodeData(data ,&(f->data),2);
    f->lastSend = 0;
    f->maxRetry = 0;
    _AckToSend.push_back(*f);
}

void Communication::sendBuffer( HEADER_Protocol header, BYTE* data, int length, bool needAck, int nretry ){
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
