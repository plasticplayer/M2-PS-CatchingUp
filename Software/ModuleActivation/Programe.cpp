#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arduino.h>
#include "Programe.h"
#include "debug.h"
#include "LinkedList.h"

Communication * Programe::communicationProtocol = NULL;
word Programe::_idRespParking = 0x00;
byte Programe::_adresseRPI = 0x00;
byte Programe::_adresseArduino = 0x00;
byte * Programe::bufferSend = NULL;
byte Programe::bufferSendLenght = 0;
byte Programe::countParking = 0;

LinkedList<MessageProtocol*> Programe::fileMessageToSend = LinkedList<MessageProtocol*>();
LinkedList<MessageProtocol*> Programe::fileMessageWaitAck = LinkedList<MessageProtocol*>();
FuncChange Programe::_PtrFunction = NULL;

Programe::Programe()
{  
  communicationProtocol = new Communication(0x10,0x22,0x20);

  //communicationProtocol->setFunction( &__callback_default , 0x00);
  communicationProtocol->setFunction( &__callback_POOL_PARKING , POOL_PARKING);
  communicationProtocol->setFunction( &__callback_ATTRIB_ADRESS , ATTRIB_ADRESS);
  communicationProtocol->setFunction( &__callback_AUTH_RESP , AUTH_RESP);
  communicationProtocol->setFunction( &__callback_STATUS_ASK , STATUS_ASK);
  communicationProtocol->setFunction( &__callback_STATUS_RESP , STATUS_RESP);
  communicationProtocol->setFunction( &__callback_ACK , ACK);
  _idRespParking = 0x0000;
}

MessageProtocol * Programe::createMessage(HeaderProtocol head,byte * data, byte len)
{
  MessageProtocol * mess = new MessageProtocol();
  mess->header = head;
  mess->lenght = len;
  if(len > 0)
  {
    mess->data = (byte * ) malloc(len * sizeof(byte));
    memcpy(mess->data,data,len);
  }
  mess->needAck = isAckNeeded(head);
  mess->time = 0;
  mess->sendCounter = 0;
  return mess;
}
void Programe::deleteMessage(MessageProtocol * mess)
{
  if(mess != NULL)
  {
    if(mess->lenght != 0)
      free(mess->data); 
    delete(mess);
    mess = NULL;
  }
}
boolean Programe::pushWaitAck(MessageProtocol * mess)
{
  return fileMessageWaitAck.add(mess);
}
boolean Programe::pushToSend(MessageProtocol * mess)
{
  return fileMessageToSend.add(mess);
  /* if(countToSend < SIZE_FILE_SEND)
   {
   
   fileMessageToSend[indexPushToSend] = mess;
   indexPushToSend = (indexPushToSend+1)%SIZE_FILE_SEND ;
   countToSend++;
   return true;
   }
   else return false;*/
}

boolean Programe::isToSendEmpty()
{
  return fileMessageToSend.size() == 0;
}
boolean Programe::isWaitAckEmpty()
{
  return fileMessageWaitAck.size() == 0;
}
MessageProtocol * Programe::getToSend()
{
  return fileMessageToSend.shift();
  // if(isEmpty())
  //return MessageProtocol;
  /*countToSend--;
   MessageProtocol mess = fileMessageToSend[indexPopToSend];
   indexPopToSend = (indexPopToSend+1)%SIZE_FILE_SEND;
   return mess;*/
}
MessageProtocol * Programe::getWaitAck()
{
  return fileMessageWaitAck.shift();
  // if(isEmpty())
  //return MessageProtocol;
  /* countWaitAck--;
   
   MessageProtocol mess = fileMessageWaitAck[indexPopWaitAck];
   indexPopWaitAck = (indexPopWaitAck+1)%SIZE_FILE_SEND;
   return mess;*/
}
int Programe::getWaitAckSup(unsigned long time,MessageProtocol * &ptr)
{
  unsigned long now = millis();
  for(byte i = 0 ; i < fileMessageWaitAck.size() ; i++)
  {
    MessageProtocol * mess = fileMessageWaitAck.get(i);
    if(now - mess->time >= time)
    {
      ptr = mess;
      return i;
    }
  }
  return -1;
}
int Programe::getWaitAckHeader(HeaderProtocol head,MessageProtocol * &ptr)
{
  for(byte i = 0 ; i < fileMessageWaitAck.size() ; i++)
  {
    MessageProtocol * mess = fileMessageWaitAck.get(i);
    if(head == mess->header)
    {
      ptr = mess;
      return i;
    }
  }
  return -1;
}
void Programe::deleteMessageWaitAck(int index)
{
  deleteMessage(fileMessageWaitAck.remove(index));
}

boolean Programe::isAckNeeded(HeaderProtocol head)
{
  switch (head)
  {
  case ATTRIB_OK:
  case AUTH_RESP:
  case START_REC:
  case STATUS_RESP:
    return true;
  default:
    return false;
  }
}
byte Programe::prepareMessage(MessageProtocol message,byte * &data)
{
  byte datas[message.lenght +1];
  datas[0] = message.header;
  for(int i = 0 ; i < message.lenght;i++)
    datas[i+1] = message.data[i];

  bufferSendLenght = communicationProtocol->encodeData(datas, &bufferSend, message.lenght+1);

  debugPrint( "--> ");
  for ( int i =0; i < bufferSendLenght; i++ )
  {
    debugPrintHex(bufferSend[i]);
    debugPrint ( " ");
  }
  debugPrintln ( " ");
  data = bufferSend;
  return bufferSendLenght;
}
void Programe::receiveMessage(byte * data,byte len)
{
  debugPrint( "<-- ");
  for ( int i =0; i < len; i++ )
  {
    debugPrintHex(data[i]);
    debugPrint ( " ");
  }
  debugPrintln ( " ");
  communicationProtocol->recieveData(data,len);
}


void Programe::setAddressRPI(uint64_t addr)
{
  _adresseRPI = addr;
}
void Programe::setAddressARD(uint64_t addr)
{
  _adresseArduino = addr;
}

uint64_t Programe::getAddressRPI()
{
  return _adresseRPI ;
}
uint64_t Programe::getAddressARD()
{
  return _adresseArduino;
}
void Programe::setFunctChangeAddr(FuncChange func)
{
  _PtrFunction = func;
}

/* PRIVATE */
void Programe::__callback_default( BYTE data[], int size ){
  debugPrintln("Message non connu recu !");
  for ( int i =0; i < size; i++ )
    debugPrintHex( data[i] );
}

void Programe::__callback_POOL_PARKING( BYTE data[], int size ){
  debugPrintln("Message POOL_PARKING recu");
  if(_idRespParking == 0x0000)
  {
    countParking = 0;
    _idRespParking =  random(0, 0xFFFF) & 0xFFFF;
    byte data[] = {
      _idRespParking >> 8, _idRespParking & 0xFF        };
    MessageProtocol * mes = createMessage(RESP_PARKING,data,sizeof(data));
    pushToSend(mes);
  }
  else
  {
    debugPrintln("Message POOL_PARKING recu alors que la reponse a deja ete envoye");
    if(countParking++ >= 5)
      _idRespParking = 0;
  }
}

void Programe::__callback_ATTRIB_ADRESS( BYTE data[], int size ){
  debugPrintln("Message ATTRIB_ADRESS recu");
  if(_idRespParking == word(data[0],data[1]))
  {
    _idRespParking = 0;

    _adresseRPI = (uint64_t)data[2] << 24 | (uint64_t)data[3] << 16 | (uint64_t)data[4] << 8 | data[5]  ;
    _adresseArduino = (uint64_t)data[6] << 24 | (uint64_t)data[7] << 16 | (uint64_t)data[8] << 8 | data[9]  ;
    MessageProtocol * mes = createMessage(ATTRIB_OK,NULL,0);
    pushToSend(mes);

    MessageProtocol * tmp;
    byte index = getWaitAckHeader(RESP_PARKING,tmp);
    if(index != -1)
      deleteMessageWaitAck(index);
  }
  else
  {
    debugPrintln("Message ATTRIB_ADRESS avec mauvais ID genere :");
    debugPrint("Attendu : ");
    debugPrintHex(_idRespParking);
    debugPrint(" recu : ");
    debugPrintHex(word(data[0],data[1]));


  }
}

void Programe::__callback_AUTH_RESP( BYTE data[], int size ){
  debugPrintln("Message AUTH_RESP recu");
  byte dataAck[] = {
    AUTH_RESP        };
  MessageProtocol * mes = createMessage(ACK,dataAck,sizeof(dataAck));
  pushToSend(mes);

  MessageProtocol * tmp;
  byte index = getWaitAckHeader(AUTH_ASK,tmp);
  if(index != -1)
    deleteMessageWaitAck(index);

  if(data[0] == 0x01) // Connexion authorisée
  {
    byte data[] ={
      0x01                };
    MessageProtocol * mes = createMessage(START_REC,data,sizeof(data));
    pushToSend(mes);
  }
  else // Connexion refusée
  {

  }
}

void Programe::__callback_STATUS_ASK( BYTE data[], int size ){
  debugPrintln("Message STATUS_ASK recu");
  byte dataAck[] = {
    0xAD        };
  MessageProtocol * mes = createMessage(STATUS_RESP,dataAck,sizeof(dataAck));
  pushToSend(mes);
}

void Programe::__callback_STATUS_RESP( BYTE data[], int size ){
  debugPrintln("Message STATUS_RESP recu");
  MessageProtocol * tmp;
  byte index = getWaitAckHeader(STATUS_ASK,tmp);
  if(index != -1)
    deleteMessageWaitAck(index);
}

void Programe::__callback_ACK(BYTE data[], int size ){
  debugPrintln("Message ACK recu");
  MessageProtocol * tmp;
  byte index = getWaitAckHeader((HeaderProtocol)data[0],tmp);
  if(index != -1)
    deleteMessageWaitAck(index);

  if((HeaderProtocol)data[0] == ATTRIB_OK)
    if(_PtrFunction != NULL)
      _PtrFunction(_adresseRPI,_adresseArduino);
}
























