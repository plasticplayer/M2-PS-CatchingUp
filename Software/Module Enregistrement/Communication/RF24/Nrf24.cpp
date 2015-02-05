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

Nrf24::Nrf24() : Communication( 0x10,0x22,0x20 ){
    //_Continue = true;

    cout << "Start NRF24" << endl;
    _Radio = new RF24(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

    // Radio pipe addresses for the 2 nodes to communicate.
    unsigned char pipes[][6] = {"1NODE","2NODE"};

    _Radio->begin();

    _Radio->setRetries(15,15);

    _Radio->setChannel(75);
    _Radio->setAutoAck(1);
    _Radio->setCRCLength(RF24_CRC_8);
    _Radio->setDataRate(RF24_1MBPS);
    _Radio->enableDynamicPayloads();

    _Radio->printDetails();

    _Radio->openWritingPipe(pipes[1]);
    _Radio->openReadingPipe(1,pipes[0]);
    _Radio->startListening();

    pthread_create(&_Thread , NULL, &(this->getData), (void *) this) ;
}

bool isTime( time_t ref ){
  time_t now;
  double seconds;

  time(&now);  /* get current time; same as: now = time(NULL)  */

  seconds = difftime(now,ref);

  return ( seconds > TIME_RETRY_S );
}

void* Nrf24::getData( void * r ){
    Nrf24 *nrf = ( Nrf24 * ) r;
    list<Frame> *llist = &( nrf->_FrameToSend );
    RF24 *radio = nrf->_Radio;


    char *BUFF = NULL;

    while ( nrf->state != STAND_BY ){

        if ( radio->available() )
        {
            if ( BUFF != NULL )
                free(BUFF);

            int payloadSize = radio->getDynamicPayloadSize();
            //cout << "PayLoadSize: " <<  << endl;
            BUFF = ( char * ) malloc( sizeof(char)*payloadSize);
            memset(BUFF,0, payloadSize );

            radio->read( &BUFF, payloadSize );

            nrf->recieveData( (BYTE *) BUFF,payloadSize );


            printf("Recieve: ");
            for (int i = 0; i < payloadSize; i ++){
                printf("%02x ", BUFF[i] );
            }
            printf("\n");
        }

		if ( llist->size() > 0 ){
			Frame f = llist->front();
			if ( f.lastSend == NULL || isTime(f.lastSend) ){
				cout << "Send Frame" << endl;
				radio->stopListening();
				radio->write( &(f.data), f.size );
                radio->startListening();
				llist->pop_front();

				if ( f.needAck ){
				    time(&(f.lastSend));
                    llist->push_front(f);
                    nrf->state = WAIT_ACK;
				}
			}
		}
        usleep(50000);
    }
}
