#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arduino.h>
#include "Programe.h"
#include "debug.h"
#include "LinkedList.h"
#include "led.h"

// Definition des PINS utilisés
#define LED_ROUGE_PIN  11
#define LED_VERTE_PIN  12
#define BTN_PIN        7
#define COUNT_BTN_LONG 5000
#define COUNT_BTN_COURT 100

#define LED_V_BLINK    leds.setSlowV();
#define LED_V_BLINK_F  leds.setFastV();
#define LED_V_ON       leds.setOnV();
#define LED_V_OFF      leds.setOffV();

#define LED_R_BLINK    leds.setSlowR();
#define LED_R_BLINK_F  leds.setFastR();
#define LED_R_ON       leds.setOnR();
#define LED_R_OFF      leds.setOffR();



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
unsigned long Programe::lastMessRec = -60000;
boolean Programe::inParking = false;
boolean Programe::error = false;
unsigned long Programe::counter =0;
LinkedList<MessageProtocol*> Programe::fileMessageToSend = LinkedList<MessageProtocol*>();
LinkedList<MessageProtocol*> Programe::fileMessageWaitAck = LinkedList<MessageProtocol*>();
FuncChange Programe::_PtrFunction = NULL;
FuncResetAddr Programe::_PtrFunctionResetAddr = NULL;

led Programe::leds(LED_ROUGE_PIN,LED_VERTE_PIN);

Programe::Programe()
{  
  pinMode(BTN_PIN, INPUT_PULLUP);  
  leds.init();
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
  // MessageProtocol * mess;
  // int indexMessage;

  if(inParking) // On est en parking alors on clignote les deux
  {
    LED_V_BLINK;
    LED_R_BLINK;
  }
  else if(connexionOk && error  ) // Erreur d'authentification 
  {
    if( millis()  - counter > 5000)
    {
      error = false;
    }
    LED_V_OFF;
    LED_R_BLINK_F;
  }
  else if(connexionOk && recording) // On est en train d'enregistrer
  {
    LED_V_ON;
    LED_R_OFF;
  }
  else if(!connexionOk) // Pas connecté ?
  {
    LED_V_OFF;
    LED_R_BLINK;
  }
  else // On est connecté, on est pas en erreur et pas en cours d'enregistrement
  {
    LED_V_BLINK;
    LED_R_OFF;
  }


  /* if(connexionOk) // Si on est connecté (pas d'erreur d'ack)
   {
   if((indexMessage = getWaitAckSup(2000,mess)) == -1) // Si il y a pas de message en attente
   {
   if(inParking) // on est en parking
   {
   //ledVerte.speed(0);
   //ledVerte.off();
   LED_V_BLINK;
   LED_R_BLINK;
   //ledRouge.speed(500);
   }
   else
   {
   // Dans tout les cas on eteint la led rouge
   //ledRouge.speed(0);
   //ledRouge.off();
   
   if(error)
   {
   //ledRouge.on();
   //ledRouge.speed(100);
   //ledVerte.speed(0);
   //ledVerte.off();
   LED_R_BLINK;
   LED_V_OFF;
   
   }
   else if(!recording) // Si on est pas en train d'enregistrer
   {
   LED_R_OFF;
   LED_V_BLINK;
   }
   else // Si on est en train d'enregistrer
   {
   LED_R_OFF;
   LED_V_ON;
   }
   }
   }
   else // Si on a un message en attente 
   {
   LED_R_ON;
   LED_V_OFF;
   }
   }
   else // Si on est pas connecte 
   {
   if(inParking) // on est en parking
   {
   ledRouge.speed(500);
   }
   else
   {
   ledRouge.speed(100);
   }
   ledVerte.speed(0);
   ledVerte.off();
   }*/

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
        LED_V_ON;
        LED_R_ON;
        /*ledVerte.speed(0);
         ledVerte.on();
         ledRouge.speed(0);
         ledRouge.on();*/
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
            0x00          };
          MessageProtocol * mes = createMessage(START_REC,data,sizeof(data));
          pushToSend(mes);
        }
      }
      countPress = 0;
    }
  }

  leds.run(); 
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
  debugPrint(F("Create Message \n"));
  MessageProtocol * mess = new MessageProtocol();
  mess->header = head;
  mess->lenght = len;
  if(len > 0)
  {
    mess->data = (byte * ) malloc(len * sizeof(byte));
    if(mess->data != NULL)
      memcpy(mess->data,data,len);
    else
      return NULL;
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
  if(mess != NULL)
    return fileMessageWaitAck.add(mess);
  else return false;
}
boolean Programe::pushToSend(MessageProtocol * mess)
{
  if(mess != NULL)
    return fileMessageToSend.add(mess);
  else return false;
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
}
MessageProtocol * Programe::getWaitAck()
{
  return fileMessageWaitAck.shift();
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
    //case AUTH_RESP:
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
        _idRespParking >> 8, _idRespParking & 0xFF                                                                                          };
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
  /* byte dataAck[] = {
   AUTH_RESP                                  };*/
  // MessageProtocol * mes = createMessage(ACK,dataAck,sizeof(dataAck));
  //pushToSend(mes);
  if(data[0] == 0x01) // Connexion authorisée
  {
    error = false;
    byte data2[] ={
      0x01                    };
    MessageProtocol * mes = createMessage(START_REC,data2,sizeof(data2));
    pushToSend(mes);
  }
  else // Connexion refusée
  {
    error = true;
    counter = millis();
  }
  MessageProtocol * tmp;
  byte index = getWaitAckHeader(AUTH_ASK,tmp);
  if(index != -1)
    deleteMessageWaitAck(index);
}

void Programe::__callback_STATUS_ASK( BYTE data[], int size ){
  debugPrintln("Message STATUS_ASK recu");
  recording = (data[0] == 0x01);
  MessageProtocol * mes = createMessage(STATUS_RESP,NULL,0);
  pushToSend(mes);
}

void Programe::__callback_STATUS_RESP( BYTE data[], int size ){
  debugPrintln("Message STATUS_RESP recu");
  recording = (data[0] == 0x01);
  MessageProtocol * tmp;
  byte index = getWaitAckHeader(STATUS_ASK,tmp);
  if(index != -1)
    deleteMessageWaitAck(index);
  byte dataAck[] = {
    STATUS_RESP                                            };
  MessageProtocol * mes = createMessage(ACK,dataAck,sizeof(dataAck));
  pushToSend(mes);
  error = false;
}

void Programe::__callback_ACK(BYTE data[], int size ){
  debugPrintln("Message ACK recu");
  MessageProtocol * tmp;
  byte index = getWaitAckHeader((HeaderProtocol)data[0],tmp);
  error = false;
  if(index != -1)
  {
    deleteMessageWaitAck(index);

    if((HeaderProtocol)data[0] == START_REC )
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
}




