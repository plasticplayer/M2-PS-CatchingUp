#ifndef __CallBack__Communication__
#define __CallBack__Communication__

#define ADDR_PARKING_TX 0xF2000000LL
#define ADDR_PARKING_RX 0xF1000000LL

#include "Udp.h"
#include "Tcp.h"







bool initRecording();
void initNrfCallBacks();
void initUdpCallBacks();
void initTcpCallBacks();

void functionNotFound( BYTE data[], int size );
void transactionError( BYTE data[], int size );

void ParringNrf();
void REC_TO_SRV_allFilesSends( uint64_t idRecording );
void REC_TO_SRV_StorageAsk( unsigned long fileInQueue );
void REC_TO_SRV_EndFileTransfert( uint64_t chksum, char* fileName, BYTE nameLength );

bool ftpCheck();
void ftpSenderStart();

void forceStartRecording();
void forceStopRecording();
#endif
