//
//  Nrf24.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 30/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Nrf24.h"

#include <stdio.h>

#include <stdlib.h>
#include <iostream>

using namespace std;

Nrf24::Nrf24( ) : Communication( 0x10,0x22,0x20 ){
	_UpdateAddr = false;
	_Address[0] = 0x00000024LL;
	_Address[1] = 0x00000035LL;

    _Error = NULL;

    cout << "Start NRF24" << endl;
    _Radio = new RF24(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);


    _Radio->begin();

    _Radio->setRetries(15,15);

    _Radio->setChannel(75);
    _Radio->setAutoAck(1);
    _Radio->setCRCLength(RF24_CRC_8);
    _Radio->setDataRate(RF24_1MBPS);
    _Radio->enableDynamicPayloads();

    _Radio->printDetails();

    _Radio->openWritingPipe(_Address[1]);
    _Radio->openReadingPipe(1,_Address[0]);
    _Radio->startListening();

    pthread_create(&_Thread , NULL, &(this->getData), (void *) this) ;
}

int isTime( time_t ref ){
  time_t now;
  //double seconds;

  time(&now);  /* get current time; same as: now = time(NULL)  */
  //seconds = ;
  return (int)difftime(now,ref);
}

void Nrf24::updateAddr( uint64_t rx, uint64_t tx ){
	_Address[0] = rx;
	_Address[1] = tx;
	_UpdateAddr = true;
}

void Nrf24::SendPoolingFrame(){
    cout << "Add Pooling Frame"  << endl;
    time(&_LastFrameRecieve);

    BYTE DAT[] = {STATUS_ASK,0x02};
    sendBuffer( STATUS_ASK, DAT, sizeof(DAT), false, 10 );
}

void* Nrf24::getData( void * r ){
    Nrf24 *nrf = ( Nrf24 * ) r;
    list<Frame> *llist = &( nrf->_FrameToSend );
    RF24 *radio = nrf->_Radio;


    char *BUFF = NULL;

    while ( true ){
		if ( nrf->_UpdateAddr == true ){
			radio->stopListening();

			usleep(10000);

			radio->openWritingPipe(nrf->_Address[1]);
			radio->openReadingPipe(1,nrf->_Address[0]);

            nrf->_Connected = false;
			radio->startListening();
			radio->printDetails();

			nrf->_UpdateAddr = false;
		}
        if ( radio->available() )
        {
            if ( BUFF != NULL )
                free(BUFF);

            int payloadSize = radio->getDynamicPayloadSize();
            //cout << "PayLoadSize: " <<  payloadSize << endl;

            BUFF = ( char * ) malloc( sizeof(char)*payloadSize);

            memset(BUFF,0, payloadSize );


            radio->read( BUFF, payloadSize );

            printf("<-- ");
            for (int i = 0; i < payloadSize; i ++){
                printf("%02x ", (unsigned char) BUFF[i] );
            }
            cout << endl;

            nrf->recieveData( (BYTE *) BUFF,payloadSize );
        }

        if ( isTime( nrf->_LastFrameRecieve ) > TIME_POOLING && llist->size() == 0 ){

            nrf->SendPoolingFrame();

        }

		if ( llist->size() > 0 ){
			Frame f = llist->front();
			if ( isTime( f.lastSend ) > TIME_RETRY ){
				radio->stopListening();

                printf("--> ");
                for (int i = 0; i < f.size; i ++){
                    printf("%02x ", (unsigned char) f.data[i] );
                }
                cout << endl;
                int maxRetry = 5;
				while ( 1!= radio->write( (f.data), f.size ) && maxRetry-- ){
				    usleep(5000);
				    if ( maxRetry == 0 && nrf->_Error != NULL){
                            nrf->_Error( (BYTE*) (f.header), 1);
				    }
                };
                radio->startListening();
				llist->pop_front();

				if ( f.maxRetry > 0 ){
				     f.maxRetry--;
				     time(&(f.lastSend));
                     llist->push_front(f);
				}
			}
		}
        usleep(1000);
    }
    return NULL;
}

void Nrf24::setTransactionErrorFunction( void (*FuncType)( BYTE data[], int size) ){
    _Error = FuncType;
}
