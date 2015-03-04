#include <stdio.h>
#include <stdlib.h>
#include "Nrf24.h"
#include "CallBack.h"
#include "Communication.h"

using namespace std;

typedef unsigned char BYTE ;

bool isRecording = false;

Nrf24 *nrfCom ;
bool isParking = false;
StateTransaction state = WAIT_FOR_DATA;


BYTE idGenerate[2];
uint64_t addressGenerate[2];


void transactionError( BYTE data[], int size ){
    cout << "Transaction Error" << endl;
}
void appairage(){
    cout << "Start Appairage" << endl;
    nrfCom->clearCommunications();
    isParking = true;

    nrfCom->updateAddr(ADDR_PARKING_RX,ADDR_PARKING_TX, false);
	BYTE answer[] = { POOL_PARK_ADDR } ;
    nrfCom->sendBuffer(POOL_PARK_ADDR,answer,1,false, 10);
}

void getAck(BYTE data[], int size){
    if ( size == 0 )
        return;

    cout << "ACK " << (int) data[0] << endl;
    if ( nrfCom->_FrameToSend.empty() )
        return;

    Frame f = nrfCom->_FrameToSend.front();
    if ( f.header == data[0] )
        nrfCom->_FrameToSend.pop_front();
}

void setComm( Nrf24* nr ){
    nrfCom = nr;
}

void authentificationRequest( BYTE data[], int size ){
    if ( isParking == true || state != WAIT_FOR_DATA ){
        return;
    }

    cout << "Authentification request. (Tag Id: ";
    for ( int i =0; i < size; i++ )
        printf("%02x ", data[i]);
    cout << ")"<< endl;

    bool status = true;

    BYTE answer[] = { AUTH_ANS, ( status )?0x01:0x00 } ;
    nrfCom->sendBuffer(AUTH_ANS,answer,2,false, 0);
}

void statusRequest( BYTE data[], int size ){
    if ( isParking == true ){
        cout << "Status request ignored" ;
        return;
    }

    cout << "Status request " ;
    if ( size == -1 ){
        BYTE answer[] = {  STATUS_ASK, ( isRecording )?0x01:0x00 } ;
        nrfCom->sendBuffer(STATUS_ASK, answer, sizeof(answer), false, 0);
        return;
    }
    BYTE answer[] = {  STATUS_ANS, ( isRecording )?0x01:0x00 } ;
    nrfCom->sendBuffer(STATUS_ANS, answer, sizeof(answer), true, 0);

}

void statusAns( BYTE data[], int size ){
    if ( isParking == true ){
        cout << "Status Ans ignored"  << endl ;
        return;
    }
    cout << "Status Ans"  << endl ;

	if ( ! nrfCom->_FrameToSend.empty()  ){
   	 	Frame f =  nrfCom->_FrameToSend.front();
   	 	if ( f.header == STATUS_ASK )
   	 	{
        	nrfCom->_FrameToSend.pop_front();
			state = WAIT_FOR_DATA;
		}
	}
    nrfCom->sendAck(STATUS_ANS);
}

void ControlRequest( BYTE data[], int size ){
    if ( isParking == true || state != WAIT_FOR_DATA ){
        cout << "Control request ignored";
        return;
    }
    cout << "Control request " << data[0] << endl ;
    isRecording = ( data[0] == 0x01) ;
    nrfCom->sendAck(ACTIVATE_RECORDING);
}

void AppariagePoolingStep1(BYTE data[], int size){
    if ( isParking == false ){
        return;
    }
    // TODO: N'authoriser qu'une suele réponse par demande d'appairage
    nrfCom->clearCommunications();

	printf(" Appairage ID %02x %02x\n", data[0], data[1] );

	memcpy(idGenerate,data,2);

	addressGenerate[0] = (uint64_t) ( rand()<<32 | rand()<<16 | rand() ) & 0x00FFFFFFFF;
	addressGenerate[1] = (uint64_t) ( rand()<<32 | rand()<<16 | rand() ) & 0x00FFFFFFFF ;

    BYTE answer[] = {  	POOL_SET_ADDR , idGenerate[0] , idGenerate[1] ,
    					(addressGenerate[0]>>24) & 0xFF, (addressGenerate[0]>>16) & 0xFF,  (addressGenerate[0]>>8) & 0xFF,  (addressGenerate[0] & 0xFF),
						(addressGenerate[1]>>24) & 0xFF , (addressGenerate[1]>>16) & 0xFF,  (addressGenerate[1]>>8) & 0xFF,  (addressGenerate[1] & 0xFF),
					} ;
    nrfCom->sendBuffer(POOL_SET_ADDR, answer ,sizeof(answer) , false, 5);
}

void AppariageValidConfig(BYTE data[], int size){
   if ( isParking == false ){
        return;
   }
   isParking = false;
   nrfCom->clearCommunications();

   nrfCom->sendAck(POOL_VALIDATE_CONFIG);
   usleep(2500000);
   nrfCom->updateAddr( addressGenerate[0], addressGenerate[1], true );
}
