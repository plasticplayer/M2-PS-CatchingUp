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
#include "CallBack.h"
#include <list>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cctype>
#include "INIReader.h"

typedef unsigned char BYTE ;
using namespace std;


int main(int argc, const char * argv[]) {

	cout << "Start Programm" << endl;
	srand(time(NULL));
	uint64_t server, client;

    INIReader reader("./config.ini");
    if(reader.ParseError() < 0)
    {
        cout << "Erreur lecture fichier INI"<<endl;
        return -1;
    }

    string s = reader.Get("config","server","").c_str();
    char *S = new char[s.length() + 1];
    std::strcpy(S,s.c_str());
    server = strtoull(S, NULL, 16);

    s = reader.Get("config","client","").c_str();
    S = new char[s.length() + 1];
    std::strcpy(S,s.c_str());
    client = strtoull(S, NULL, 16);

    cout << "Serveur : " << server << " | Client : "<< client <<endl;


    Nrf24* nrfCom = new Nrf24();
    //nrfCom->start();
    //nrfCom->updateAddr(  0x123456789LL, 0x0987654321LL, false );
    nrfCom->updateAddr(  client, server,  false );
    nrfCom->start();
    setComm( nrfCom );

    nrfCom->setFunction(&getAck,ACK);
    nrfCom->setFunction(&authentificationRequest, AUTH_ASK);
    nrfCom->setFunction(&statusRequest, STATUS_ASK);
    nrfCom->setFunction(&statusAns, STATUS_ANS);
    nrfCom->setFunction(&ControlRequest, ACTIVATE_RECORDING);
    nrfCom->setFunction(&AppariagePoolingStep1, POOL_PARK_ADDR_ANS);
	nrfCom->setFunction(&AppariageValidConfig, POOL_VALIDATE_CONFIG);
    nrfCom->setTransactionErrorFunction(&transactionError);



    while ( char c = getchar() ){

		if(c == 'a' )
		{
		    appairage();
            fflush(stdin);
		}
        if(c == 'q' )
		{
		   break;
		}
    }
    return 0;
}

