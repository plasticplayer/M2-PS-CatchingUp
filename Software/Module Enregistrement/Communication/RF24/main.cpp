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
#include "INIReader.h"

typedef unsigned char BYTE ;
using namespace std;


typedef struct {
        BYTE *server, *client;//, *password, *database;
       // gint port;
} Settings;


/*void readConfigFile(BYTE* &server, BYTE* &client) {

        Settings *conf;
        GKeyFile *keyfile;
        GKeyFileFlags flags;
        GError *error = NULL;
        gsize length;

        keyfile = g_key_file_new ();
        //flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

        cout << "Looking for config file " << cfgfile << endl;

        if (!g_key_file_load_from_file (keyfile, cfgfile, flags, &error)) {
                g_error (error->message);
        } else {
            cout << "config file loaded." << endl;
            conf = g_slice_new (Settings);
            conf->server    = g_key_file_get_string(keyfile, "config", "server", NULL);
            server = conf->server;
            conf->user      = g_key_file_get_string(keyfile, "config", "client", NULL);
            client = conf->user;
        }

}*/



int main(int argc, const char * argv[]) {
	int server, client;
	cout << "TnEST !!!!"<<endl<<endl;
	//readConfigFile("./config.ini", server, client);
    INIReader reader("./config.ini");
    if(reader.ParseError() < 0)
    {
        cout << "Erreur lecture fichier INI"<<endl;
        return -1;
    }
    server = reader.GetInteger("config","server",-1);
    client= reader.GetInteger("config","client",-1);

    cout << "Serveur : " << server << " | Client : "<< client <<endl;



    Nrf24* nrfCom = new Nrf24( );
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

