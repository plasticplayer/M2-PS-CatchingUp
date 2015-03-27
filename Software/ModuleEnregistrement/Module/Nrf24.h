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

class Nrf24 : public Communication{
public:

    Nrf24( );

    void start();
    void SendPoolingFrame();
    void setTransactionErrorFunction( void (*FuncType)( BYTE data[], int size) );
	void updateAddr( uint64_t rx, uint64_t tx, bool writeToFile );
    bool isOk();
private:
    void poolState();
    bool _UpdateAddr;
    bool _NFRok;
    pthread_t _Thread;
    RF24 *_Radio;



    FuncType _Error;

    static void *getData( void *);

    uint64_t _Address[2];
};

#endif /* defined(__ProtocolCommunication__Nrf24__) */
