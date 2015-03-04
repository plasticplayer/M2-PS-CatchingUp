#ifndef __CallBack__Communication__
#define __CallBack__Communication__

#define ADDR_PARKING_TX 0xF2000000LL
#define ADDR_PARKING_RX 0xF1000000LL

enum StateTransaction { WAIT_FOR_DATA, WAIT_ACK, APPAIRAGE, STAND_BY };

void functionNotFound( BYTE data[], int size );
void transactionError( BYTE data[], int size );

void setComm( Nrf24* nr );
void appairage();
void getAck(BYTE data[], int size);
void statusRequest( BYTE data[], int size );
void authentificationRequest( BYTE data[], int size );
void statusAns( BYTE data[], int size );
void ControlRequest( BYTE data[], int size );
void AppariagePoolingStep1(BYTE data[], int size);
void AppariageValidConfig(BYTE data[], int size);


#endif
