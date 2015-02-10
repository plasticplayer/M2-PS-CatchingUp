

#ifndef __Protocole__h__
#define __Protocole__h__
#include "Arduino.h"
#include "Communication.h"
#include "LinkedList.h"

//#define SIZE_FILE_SEND 10

typedef enum _HeaderProtocol
{
  // Appairage
  POOL_PARKING = 0x01,
  RESP_PARKING = 0x02,
  ATTRIB_ADRESS = 0x03,
  ATTRIB_OK = 0x04,
  // Auth card
  AUTH_ASK = 0x05,
  AUTH_RESP = 0x06,
  // Controle
  START_REC = 0x07,
  // Status
  STATUS_ASK = 0x0A,
  STATUS_RESP = 0x0B,
  // Acknoledge
  ACK = 0x0F
}
HeaderProtocol;

typedef struct _MessageProtocol
{
  HeaderProtocol header;
  boolean needAck;
  byte lenght;
  byte * data;
  unsigned long time;
  byte sendCounter;
}
MessageProtocol;

typedef void (*FuncChange)( uint64_t RPI, uint64_t ARDU);

class Programe
{
public :
  Programe();
  static MessageProtocol * createMessage(HeaderProtocol header,byte * data, byte lenght) ;
  static void deleteMessage(MessageProtocol * mess);
  static boolean isAckNeeded(HeaderProtocol head);
  static void receiveMessage(byte * data,byte len);
  static byte prepareMessage(MessageProtocol message,byte * &data);

  static boolean pushToSend(MessageProtocol * mess);
  static boolean isToSendEmpty();
  static MessageProtocol * getToSend();

  boolean pushWaitAck(MessageProtocol * mess);
  static boolean isWaitAckEmpty();
  static MessageProtocol * getWaitAck();
  static int getWaitAckSup(unsigned long time,MessageProtocol * &ptr);
  static int getWaitAckHeader(HeaderProtocol head,MessageProtocol * &ptr);
  static void deleteMessageWaitAck(int index);

  static void setAddressRPI(uint64_t addr);
  static void setAddressARD(uint64_t addr);
  static uint64_t getAddressRPI();
  static uint64_t getAddressARD();
  static void setFunctChangeAddr(FuncChange func);


private :
  // Callbacks
  static void __callback_default( BYTE data[], int size );
  static void __callback_POOL_PARKING( BYTE data[], int size );
  static void __callback_ATTRIB_ADRESS( BYTE data[], int size );
  static void __callback_AUTH_RESP( BYTE data[], int size );
  static void __callback_STATUS_ASK( BYTE data[], int size );
  static void __callback_STATUS_RESP( BYTE data[], int size );
  static void __callback_ACK(BYTE data[], int size );

  static Communication * communicationProtocol;
  static word _idRespParking;
  static byte _adresseRPI;
  static byte _adresseArduino;
  static byte * bufferSend;
  static byte bufferSendLenght;
  static byte countParking;

  static LinkedList<MessageProtocol*> fileMessageToSend;
  static LinkedList<MessageProtocol*> fileMessageWaitAck;
  static FuncChange _PtrFunction;
  /*static MessageProtocol fileMessageToSend[SIZE_FILE_SEND];
   static MessageProtocol fileMessageWaitAck[SIZE_FILE_SEND];
   static byte indexPushToSend;
   static byte indexPopToSend;
   static byte countToSend;
   
   static byte indexPushWaitAck;
   static byte indexPopWaitAck;
   static byte countWaitAck;*/
};

#endif





