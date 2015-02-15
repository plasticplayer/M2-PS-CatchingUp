#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arduino.h>
#include "Programe.h"
#include "debug.h"
#include "LinkedList.h"
#include "led.h"

// Definition des PINS utilisés
#define LED_ROUGE_PIN  3
#define LED_VERTE_PIN  2
#define BTN_PIN        A1
#define COUNT_BTN_LONG 5000
#define COUNT_BTN_COURT 250

#define LED_V_BLINK ledVerte.speed(500);
#define LED_V_ON ledVerte.speed(0); \
ledVerte.on();
#define LED_V_OFF ledVerte.speed(0); \
ledVerte.off();

#define LED_R_BLINK ledRouge.speed(500);
#define LED_R_ON ledRouge.speed(0); \
ledRouge.on();
#define LED_R_OFF ledRouge.speed(0); \
ledRouge.off();


Communication * Programe::communicationProtocol = NULL;
word Programe::_idRespParking = 0x00;
uint64_t Programe::_adresseRPI = 0x00;
uint64_t Programe::_adresseArduino = 0x00;
byte * Programe::bufferSend = NULL;
byte Programe::bufferSendLenght = 0;
byte Programe::countParking = 0;
boolean Programe::recording = false;
boolean Programe::connexionOk = false;
//boolean Programe::resetAddress = false;
unsigned long Programe::countPress = 0;
unsigned long Programe::lastMessRec = 0;
boolean Programe::inParking = false;

LinkedList<MessageProtocol*> Programe::fileMessageToSend = LinkedList<MessageProtocol*>();
LinkedList<MessageProtocol*> Programe::fileMessageWaitAck = LinkedList<MessageProtocol*>();
FuncChange Programe::_PtrFunction = NULL;
FuncResetAddr Programe::_PtrFunctionResetAddr = NULL;

led Programe::ledVerte(LED_VERTE_PIN);
led Programe::ledRouge(LED_ROUGE_PIN);

Programe::Programe()
{  
  pinMode(BTN_PIN, INPUT_PULLUP);  
  ledVerte.init();
  ledRouge.init();
  LED_R_ON;
  LED_V_ON;
  //delay(5);
  LED_R_OFF;
  LED_V_OFF;
  LED_V_BLINK;

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
boolean Programe::isRecording()
{
  return recording;
}
void Programe::tick()
{
  
    if(connexionOk)
  {
    if(isWaitAckEmpty())
    {
      if(!recording)
      {
        ledVerte.speed(250);
      }
      else
      {
        ledVerte.speed(0);
        ledVerte.on();
      }
      ledRouge.speed(0);
      ledRouge.off();
    }
    else
    {
      ledVerte.speed(0);
        ledVerte.off();
      
      ledRouge.speed(0);
      ledRouge.on();
    }
  }
  else
  {
    if(inParking)
    {
      ledRouge.speed(500);
    }
    else
    {
      ledRouge.speed(100);
    }
    ledVerte.speed(0);
    ledVerte.off();
  }
  
  if(digitalRead(BTN_PIN) == LOW) // Btn Appuyé
  {
    if(countPress == 0)
    {
      countPress = millis(); 
    }
    else
    {
      if(millis() - countPress >= COUNT_BTN_LONG)
      {
          ledVerte.speed(0);
        ledVerte.on();
          ledRouge.speed(0);
        ledRouge.on();
      }
      //printf("Bouton appuyé en attente : %lu\r\n", millis() - countPress);
    }
  }
  else
  {
    if(countPress != 0) // On a été appuyé
    {
      if(millis() - countPress >= COUNT_BTN_LONG)
      {
        //if(!resetAddress)
        {
          if(_PtrFunctionResetAddr != NULL)
          {
            _PtrFunctionResetAddr();
            _idRespParking = 0;
            countPress = 0;
            //resetAddress = true;
          }
        }
        //resetAddress = false;
      }
      else if(millis() - countPress >= COUNT_BTN_COURT)
      {
        if(recording)
        {
          byte data[] ={
            0x00                                                                      };
          MessageProtocol * mes = createMessage(START_REC,data,sizeof(data));
          pushToSend(mes);
        }
      }
      countPress = 0;
    }
  }




  ledVerte.run();
  ledRouge.run(); 
}
boolean Programe::isNFCneeded()
{ 
  return connexionOk && !recording && isToSendEmpty() && isWaitAckEmpty();
}
void Programe::disconnect()
{
  connexionOk = false;
  recording = false;
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
unsigned long Programe::getLastMessRec()
{
 return lastMessRec;
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
  if(communicationProtocol->recieveData(data,len))
  {
    lastMessRec = millis();
    connexionOk = true;
  }
  else
    connexionOk = false;
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
void Programe::setFunctResetAddr(FuncResetAddr reset)
{
  _PtrFunctionResetAddr = reset;
}
void Programe::setParking(uint64_t ARDU,uint64_t RPI)
{
   setAddressRPI(ARDU);
   setAddressARD(RPI);
   inParking = true;
}
boolean Programe::isInParking()
{
  return inParking;
}
/* PRIVATE */
void Programe::__callback_default( BYTE data[], int size ){
  debugPrintln("Message non connu recu !");
  for ( int i =0; i < size; i++ )
    debugPrintHex( data[i] );
}

void Programe::__callback_POOL_PARKING( BYTE data[], int size ){
  debugPrintln("Message POOL_PARKING recu");
  if(inParking)
  {
    if(_idRespParking == 0x0000)
    {
      countParking = 0;
      _idRespParking =  random(0, 0xFFFF) & 0xFFFF;
      byte data[] = {
        _idRespParking >> 8, _idRespParking & 0xFF                                                            };
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
    AUTH_RESP                                  };
  MessageProtocol * mes = createMessage(ACK,dataAck,sizeof(dataAck));
  pushToSend(mes);

  MessageProtocol * tmp;
  byte index = getWaitAckHeader(AUTH_ASK,tmp);
  if(index != -1)
    deleteMessageWaitAck(index);

  if(data[0] == 0x01) // Connexion authorisée
  {
    byte data[] ={0x01};
    MessageProtocol * mes = createMessage(START_REC,data,sizeof(data));
    pushToSend(mes);
  }
  else // Connexion refusée
  {

  }
}

void Programe::__callback_STATUS_ASK( BYTE data[], int size ){
  debugPrintln("Message STATUS_ASK recu");
  recording = (data[1] == 0x01);
  MessageProtocol * mes = createMessage(STATUS_RESP,NULL,0);
  pushToSend(mes);
}

void Programe::__callback_STATUS_RESP( BYTE data[], int size ){
  debugPrintln("Message STATUS_RESP recu");
  recording = (data[1] == 0x01);
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

  if((HeaderProtocol)data[0] == START_REC)
  {
    recording = !recording;

  }
  else if((HeaderProtocol)data[0] == ATTRIB_OK)
  {
    if(_PtrFunction != NULL)
      _PtrFunction(_adresseRPI,_adresseArduino);
     inParking = false;
  }
}





































