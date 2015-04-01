
#include "Config.h"

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
   char tmp[250];
    tmp[0] = '\0';
    for(int i = 0; i < size; i++)
        sprintf(tmp,"%s %02X",tmp,data[i]);

    LOGGER_ERROR("Transaction Error ! Recieve ["<< size <<"] :" << tmp );
}
void appairage(){
    LOGGER_INFO("Starting Appairage");
    nrfCom->clearCommunications();
    isParking = true;

    nrfCom->updateAddr(ADDR_PARKING_RX,ADDR_PARKING_TX, false);
	BYTE answer[] = { POOL_PARK_ADDR } ;
    nrfCom->sendBuffer(POOL_PARK_ADDR,answer,1,false, 10);
}

void getAck(BYTE data[], int size){
    if ( size == 0 )
        return;

    LOGGER_DEBUG("ACK " << (int) data[0]);
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
    char tmp[250];
    tmp[0] = '\0';
    for(int i = 0; i < size; i++)
        sprintf(tmp,"%s %02X",tmp,data[i]);

    LOGGER_INFO("Authentification request. (Tag Id: " << tmp <<")");

    bool status = true;
    BYTE answer[] = { AUTH_ANS, ( status )?(uint8_t)0x01:(uint8_t)0x00 } ;
    nrfCom->sendBuffer(AUTH_ANS,answer,2,false, 0);
}

void statusRequest( BYTE data[], int size ){
    LOGGER_DEBUG("Status request received");
    if ( isParking == true ){
        LOGGER_WARN("Module in Parking => Status request ignored");
        return;
    }
    if ( size == -1 ){
        BYTE answer[] = {  STATUS_ASK, ( isRecording )?(uint8_t)0x01:(uint8_t)0x00 } ;
        nrfCom->sendBuffer(STATUS_ASK, answer, sizeof(answer), false, 0);
        return;
    }
    BYTE answer[] = {  STATUS_ANS, ( isRecording )?(uint8_t)0x01:(uint8_t)0x00 } ;
    nrfCom->sendBuffer(STATUS_ANS, answer, sizeof(answer), true, 0);

}

void statusAns( BYTE data[], int size ){
    LOGGER_DEBUG("Status Ans received");
    if ( isParking == true ){
        LOGGER_WARN("Module in Parking => Status Ans ignored" );
        return;
    }

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
    LOGGER_DEBUG("Control request receieved : " << (int)data[0]);
    if ( isParking == true || state != WAIT_FOR_DATA ){
         LOGGER_WARN("Module in Parking OR Waiting for data => Control request ignored");
        return;
    }
    isRecording = ( data[0] == 0x01) ;
    nrfCom->sendAck(ACTIVATE_RECORDING);
}

void AppariagePoolingStep1(BYTE data[], int size){
    if ( isParking == false ){
        return;
    }
    // TODO: N'authoriser qu'une suele réponse par demande d'appairage
    nrfCom->clearCommunications();
    //LOGGER_DEBUG("Appairage ID recieved :" << std::uppercase << std::hex << data[0] << " " << std::uppercase << std::hex<< data[1]);


	memcpy(idGenerate,data,2);

	addressGenerate[0] = (uint64_t) ( ((uint64_t)rand())<<32 | ((uint64_t)rand())<<16 | rand() ) & 0x00FFFFFFFF;
	addressGenerate[1] = (uint64_t) ( ((uint64_t)rand())<<32 | ((uint64_t)rand())<<16 | rand() ) & 0x00FFFFFFFF ;

    BYTE answer[] = {  	POOL_SET_ADDR , idGenerate[0] , idGenerate[1] ,
    					(uint8_t)((addressGenerate[0]>>24) & 0xFF), (uint8_t)((addressGenerate[0]>>16) & 0xFF),  (uint8_t)((addressGenerate[0]>>8) & 0xFF),  (uint8_t)((addressGenerate[0] & 0xFF)),
						(uint8_t)((addressGenerate[1]>>24) & 0xFF) ,(uint8_t)((addressGenerate[1]>>16) & 0xFF),  (uint8_t)((addressGenerate[1]>>8) & 0xFF),  (uint8_t)((addressGenerate[1] & 0xFF)),
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
