//
//  Nrf24.h
//  ProtocolCommunication
//
//  Created by maxime max on 30/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#ifndef __ProtocolCommunication__Nrf24__
#define __ProtocolCommunication__Nrf24__

#include <stdio.h>

#include <pthread.h>
#include "Communication.h"
#include "RF24.h"
#include "bcm2835.h"

enum StateTransaction { WAIT_FOR_DATA, WAIT_ACK, APPAIRAGE, STAND_BY };


class Nrf24 : public Communication{
public:

    Nrf24( );
	static Nrf24* _Nrf;
	StateTransaction _State;
	
	bool isOk();
	void start();
    void SendPoolingFrame();
	void updateAddr( uint64_t rx, uint64_t tx, bool writeToFile );
	void setTransactionErrorFunction( void (*FuncType)( BYTE data[], int size) );
    
	
private:
	bool _NFRok;
	RF24 *_Radio;
	FuncType _Error;
	bool _UpdateAddr;
	pthread_t _Thread;
	uint64_t _Address[2];
	
    void poolState();
    static void *getData( void *); 
};

#endif /* defined(__ProtocolCommunication__Nrf24__) */
