#include "Programe.h"
#include "debug.h"
// On inclue les fichiers pour l'utilisation du lecteur de carte RFID
#include <PN532_HSU.h>
#include <PN532.h>
// On inclue les fichiers pour l'utilisation des modules NRF24L01
#include <SPI.h>
#include <EEPROM.h>
#include <nRF24L01.h>
#include <RF24.h>
// Utilitaires
#include "printf.h"

#define MAX_RESEND 5 // 5 envois max (1 + 4 rejeux)
#define RESEND_DELAY 2000 // ms
#define EEP_ADR_SET 0x0F
#define EEP_ADR_RPI 0x1F
#define EEP_ADR_ARD 0x2F
// Channal utilise pour la transmission RF
#define RF_CHAN  75
// Debit RF
#define RF_BANDWITH RF24_1MBPS
// Utilisation d'un CRC pour le CRC
#define RF_CRC RF24_CRC_8
// Adresses pour le NRF 24
#define ADRESSE_RECEPTION_PARKING  0x00000001LL
#define ADRESSE_EMISSION_PARKING   0x00000002LL
// Pins utilisés par le RFID
#define RF_CE_PIN 8
#define RF_CSN_PIN 10

// Definition des PINS utilisés
#define LED_ROUGE_PIN  3
#define LED_VERTE_PIN  2
#define BTN_PIN  A2

// Lecteur de carte RFID
PN532_HSU pn532(Serial1);
PN532 nfc(pn532);

// Module Radio NRF24
RF24 radio(RF_CE_PIN,RF_CSN_PIN);

         

// Dernier Tag lu par le lecteur RFID
uint8_t tagRead[] = {
  0,0,0,0,0,0,0        };


Programe prog;

void setup(void) {
  randomSeed(analogRead(0));
  pinMode(LED_ROUGE_PIN, OUTPUT);     
  pinMode(LED_VERTE_PIN, OUTPUT);     
  pinMode(BTN_PIN, INPUT_PULLUP);  

#ifdef DEBUG_ON
  Serial.begin(9600);
  while (!Serial);
  while( Serial.read() != 'S')
  {
    debugPrintln("Debug Mode Active. Presser sur S pour demarrer le programme");
    delay(1000);
  }
  printf_begin();
#endif

  nfc.begin();
#ifdef DEBUG_ON
  uint32_t versiondata = nfc.getFirmwareVersion();
  debugPrint("Version lecteur RFID :");
  debugPrintln(versiondata);
#endif

  nfc.SAMConfig();

  radio.begin();                          // Start up the radio
  radio.setAutoAck(1);                    // Ensure autoACK is enabled
  radio.setChannel(RF_CHAN);
  radio.setCRCLength(RF_CRC);
  radio.setRetries(15,15);                // Max delay between retries & number of retries
  radio.setDataRate(RF_BANDWITH);
  readEEPAdresses();
  changeNRFAdresses();
  radio.enableDynamicPayloads();
  radio.startListening();                 // Start listening
#ifdef DEBUG_ON
  radio.printDetails();                 // Dump the configuration of the rf unit for debugging
#endif

  prog.setFunctChangeAddr(&changeAdresses);
}

byte * buffToSend = NULL;
void loop(void) {


  if(radio.available()) // Quelque chose de disponnible ?
  {
    byte len = radio.getDynamicPayloadSize(); 
    byte datas[len];
    radio.read(datas,len);
    prog.receiveMessage(datas,len);
  }

  if(!prog.isToSendEmpty()) // on a quelque chose à envoyer ?
  {
    MessageProtocol * mess = prog.getToSend();
    //printf("messageToSend : %X \r\n" , mess);
    if(sendMessage(mess))
      prog.pushWaitAck(mess);
  }
  MessageProtocol * mess;
  int indexMessage;
  if((indexMessage = prog.getWaitAckSup(RESEND_DELAY,mess))!= -1)
  {
    //debugPrintln("Message en attente !");
    //printf("messageToSend : %X \r\n" , mess);
    //debugPrintln(mess->sendCounter);
    if(mess->sendCounter >= MAX_RESEND)
    {
      debugPrintln("Message en attente de Ack to resend en erreur renvoye trop de fois");
      prog.deleteMessageWaitAck(indexMessage);
    }
    else
    { 
      debugPrintln("Message en attente de Ack to resend");
      sendMessage(mess);
    }
  }

  if(newTagAvailable())
  {
    MessageProtocol * mess2 = prog.createMessage(AUTH_ASK,tagRead,7);
    prog.pushToSend(mess2);
  }

}


boolean sendMessage(MessageProtocol * mess)
{
  byte length = prog.prepareMessage(*mess,buffToSend);
  radio.stopListening();

  radio.write(buffToSend,length);
  radio.startListening();
  mess->time = millis();
  mess->sendCounter++;
  if(mess->header == ACK)
    return false;
  else
    return true;

}



boolean newTagAvailable()
{
  static uint8_t Olduid[] = { 
    0, 0, 0, 0, 0, 0, 0            }; 
  boolean success = false;
  boolean bNew = false;
  uint8_t uid[] = { 
    0, 0, 0, 0, 0, 0, 0                                       };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength,250);
  if (success) {
    for(int i = 0 ; i < uidLength; i++)
    {
      if(Olduid[i] != uid[i])
      {
        // Le Tag est différent de l'ancien lu
        bNew = true;
        break;
      }
    }
    if(bNew)
    {
#ifdef DEBUG_ON
      Serial.print("  UID Length: ");
      Serial.print(uidLength, DEC);
      Serial.println(" bytes");
      Serial.print("  UID Value: ");
      for(int i = 0 ; i < uidLength; i++)
      {
        Serial.print(uid[i],HEX);
        Serial.print(" ");
      }
      Serial.println(" ");
#endif
      memset(tagRead,0,7);
      memset(Olduid,0,7);
      for(int i = 0 ; i < uidLength; i++)
      {
        Olduid[i] = uid[i];
        tagRead[7 - i-1] = uid[uidLength -1 - i];
      }
      return true;
    }
  }
  else // Il n'y a pas eu de bonne carte 
  {
    memset(Olduid,0,7);
    memset(tagRead,0,7);
  }
  return false;
}




void changeNRFAdresses()
{
  radio.stopListening();
  radio.openWritingPipe(prog.getAddressRPI());
  radio.openReadingPipe(1, prog.getAddressARD());
  radio.startListening();                 // Start listening
}
void changeAdresses(uint64_t RPI,uint64_t ARDU)
{
 printf("Changement d'adresse : 0x%X 0x%X \r\n",RPI,ARDU);
  EEPROM.write(EEP_ADR_SET,0x00);

  EEPROM.write(EEP_ADR_RPI, (RPI >> 24) & 0xFF);
  EEPROM.write(EEP_ADR_RPI+1, (RPI >> 16) & 0xFF);
  EEPROM.write(EEP_ADR_RPI+2, (RPI >> 8) & 0xFF);
  EEPROM.write(EEP_ADR_RPI+3, (RPI) & 0xFF);

  EEPROM.write(EEP_ADR_ARD, (ARDU >> 24) & 0xFF);
  EEPROM.write(EEP_ADR_ARD+1, (ARDU >> 16) & 0xFF);
  EEPROM.write(EEP_ADR_ARD+2, (ARDU >> 8) & 0xFF);
  EEPROM.write(EEP_ADR_ARD+3, (ARDU) & 0xFF);

  EEPROM.write(EEP_ADR_SET,0x01);
}
void readEEPAdresses()
{
  byte Flag = EEPROM.read(EEP_ADR_SET);

  if(Flag == 0x01) // Adresses are sets 
  {
    uint64_t addr = (uint64_t)EEPROM.read(EEP_ADR_RPI) << 24 | (uint64_t)EEPROM.read(EEP_ADR_RPI+1) << 16 | (uint64_t)EEPROM.read(EEP_ADR_RPI+2) << 8 |EEPROM.read(EEP_ADR_RPI+3);
    prog.setAddressRPI(addr);
    addr = (uint64_t)EEPROM.read(EEP_ADR_ARD) << 24 | (uint64_t)EEPROM.read(EEP_ADR_ARD+1) << 16 | (uint64_t)EEPROM.read(EEP_ADR_ARD+2) << 8 |EEPROM.read(EEP_ADR_ARD+3);
    prog.setAddressARD(EEPROM.read(EEP_ADR_ARD));
  }
  else // Switch to parking mode
  {
    prog.setAddressRPI(ADRESSE_RECEPTION_PARKING);
    prog.setAddressARD(ADRESSE_EMISSION_PARKING);
  }

 printf("Read d'adresse : 0x%X 0x%X \r\n",prog.getAddressRPI(),prog.getAddressARD());
  /*debugPrintln("Changement d'addresse !");
  debugPrint(" RPI: ");
  debugPrintln(prog.getAddressRPI(),HEX);
  debugPrint(" ARD: ");
  debugPrintln(prog.getAddressARD(),HEX);*/
}



















