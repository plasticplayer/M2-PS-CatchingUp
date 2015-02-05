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
    Nrf24();
    //static bool _Continue;

private:
    pthread_t _Thread;
    RF24 *_Radio;


    static void *getData( void *);
};

#endif /* defined(__ProtocolCommunication__Nrf24__) */
