//
//  main.cpp
//  ProtocolCommunication
//
//  Created by maxime max on 28/01/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Nrf24.h"
#include "Communication.h"
#include <list>
typedef unsigned char BYTE ;

using namespace std;


Nrf24 *nrfCom ;

void authentificationRequest( BYTE data[], int size ){
    std::cout << "Authentification request. (Tag Id: " << endl;
    for ( int i =0; i < size; i++ )
        printf("%02x ", data[i]);
    cout << ")"<< endl;

    bool status = true;

    BYTE answer[] = { ( status )?0x01:0x00 } ;
    nrfCom->sendBuffer(AUTH_ANS,answer,1,true);
}

void statusRequest( BYTE data[], int size ){
    std::cout << "Status request " ;

    BYTE answer[] = {  0x15 } ;
    nrfCom->sendBuffer(STAUTS_ANS,answer,1,true);
}

void ControlRequest( BYTE data[], int size ){
    std::cout << "Control request " << data[0] << endl ;

    nrfCom->sendAck(ACTIVATE_RECORDING);
}



int main(int argc, const char * argv[]) {
    nrfCom = new Nrf24();
    nrfCom->setFunction(&authentificationRequest, AUTH_ASK);
    nrfCom->setFunction(&statusRequest, STATUS_ASK);
    nrfCom->setFunction(&ControlRequest, ACTIVATE_RECORDING);

    //com->setFunction(&test2, 0x06);

    getchar();

    std::cout << "Hello, World!\n";
    return 0;

}

