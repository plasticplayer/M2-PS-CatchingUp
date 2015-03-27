//
//  Nrf24.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 30/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Config.h"
#include "Nrf24.h"

#include <stdio.h>

#include <stdlib.h>
#include <iostream>
#include "INIReader.h"

using namespace std;

Nrf24::Nrf24( ) : Communication( 0x10,0x22,0x20 ){
	_UpdateAddr = false;

    _Error = NULL;
    _NFRok = false;

    _Radio = new RF24(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);


    _Radio->begin();


    _Radio->setRetries(15,15);

    _Radio->setChannel(75);
    _Radio->setAutoAck(1);
    _Radio->setCRCLength(RF24_CRC_8);
    _Radio->setDataRate(RF24_1MBPS);
    _Radio->enableDynamicPayloads();

	_Radio->startListening();
     usleep(100000);

	_Radio->openWritingPipe( ADDR_PARKING_RX );
    _Radio->openReadingPipe(1,ADDR_PARKING_TX);


    if(_Radio->get_status() != 0x00)
        _NFRok = true;
}
bool Nrf24::isOk()
{
    return _NFRok;
}
void Nrf24::start(){
	pthread_create(&_Thread , NULL, &(this->getData), (void *) this) ;
}

int isTime( time_t ref ){
  time_t now;
  //double seconds;

  time(&now);  /* get current time; same as: now = time(NULL)  */
  //seconds = ;
  return (int)difftime(now,ref);
}

void Nrf24::updateAddr( uint64_t rx, uint64_t tx, bool writeToFile ){
	_Address[0] = rx;
	_Address[1] = tx;
	_UpdateAddr = true;

	if ( writeToFile == true ){
		INIReader reader("./config.ini");
	    if(reader.ParseError() < 0)
	    {
	        LOGGER_ERROR("Error writing ini file" << reader.ParseError());
	        //cout << "Erreur lecture fichier INI"<<endl;
	        return;
	    }
	    char res[10];
	    sprintf(res,"0x%02X%02X%02X%02X",(uint8_t)(((rx >> 24) & 0xFF)) , (uint8_t)((rx >> 16) & 0xFF ),(uint8_t)((rx >> 8) & 0xFF) ,(uint8_t)(rx & 0xFF));
	    reader.updateValue("config","client", res);
	    sprintf(res,"0x%02X%02X%02X%02X",(uint8_t)(((tx >> 24) & 0xFF)) , (uint8_t)((tx >> 16) & 0xFF ),(uint8_t)((tx >> 8) & 0xFF) ,(uint8_t)(tx & 0xFF));
		reader.updateValue("config","server", res);
		reader.writeIniFile("./config.ini");
	}
}

void Nrf24::SendPoolingFrame(){
    LOGGER_VERB("Adding KeepAlive frame");
    time(&_LastFrameRecieve);

    if ( this->_PtrFunctions[STATUS_ASK] != NULL ){
        this->_PtrFunctions[STATUS_ASK](NULL,-1);
    }

}

void* Nrf24::getData( void * r ){
    Nrf24 *nrf = ( Nrf24 * ) r;
    list<Frame> *llist = &( nrf->_FrameToSend ), *_Acks = &( nrf->_AckToSend );
    RF24 *radio = nrf->_Radio;

    char *BUFF = NULL;

    while ( true ){
		if ( nrf->_UpdateAddr == true ){
			radio->stopListening();

			usleep(10000);
			radio->startListening();

			radio->openWritingPipe(nrf->_Address[1]);
			radio->openReadingPipe(1,nrf->_Address[0]);

            nrf->_Connected = false;

			//radio->printDetails();

			nrf->_UpdateAddr = false;
		}
        if ( radio->available() )
        {
            if ( BUFF != NULL )
                free(BUFF);

            int payloadSize = radio->getDynamicPayloadSize();

            BUFF = ( char * ) malloc( sizeof(char)*payloadSize);

            memset(BUFF,0, payloadSize );

            radio->read( BUFF, payloadSize );

            char tmp[250];
            tmp[0] = '\0';
            for(int i = 0; i < payloadSize; i++)
                sprintf(tmp,"%s %02X",tmp, BUFF[i]);
            LOGGER_VERB("Rec " << tmp);

            nrf->recieveData( (BYTE *) BUFF,payloadSize );
        }

        if ( isTime( nrf->_LastFrameRecieve ) > TIME_POOLING && llist->size() == 0 ){

            nrf->SendPoolingFrame();

        }
        while ( _Acks->size() > 0 ){
            Frame f = _Acks->front();
            radio->stopListening();

                char tmp[250];
                tmp[0] = '\0';
                for(int i = 0; i < f.size; i++)
                    sprintf(tmp,"%s %02X",tmp, f.data[i]);
                LOGGER_VERB("Sec " << tmp);

				radio->write( (f.data), f.size );
                radio->startListening();
				_Acks->pop_front();
        }
		if ( llist->size() > 0 ){

			Frame f = llist->front();
			if ( isTime( f.lastSend ) > TIME_RETRY ){
				radio->stopListening();

                char tmp[250];
                tmp[0] = '\0';
                for(int i = 0; i < f.size; i++)
                    sprintf(tmp,"%s %02X",tmp, f.data[i]);
                LOGGER_VERB("Sending " << tmp);

				radio->write( (f.data), f.size );

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
