// On inclue les fichiers pour l'utilisation du lecteur de carte RFID
#include <PN532_HSU.h>
#include "PN532.h"
// Utilitaires
//#include "printf.h"

//#define DEBUG_ON

#define LED_ROUGE_PIN  11
#define LED_VERTE_PIN  12
#define BTN_PIN  7



// Lecteur de carte RFID
PN532_HSU pn532(Serial1);
PN532 nfc(pn532);

void setup(void) {

  pinMode(LED_ROUGE_PIN, OUTPUT);     
  pinMode(LED_VERTE_PIN, OUTPUT);     
  pinMode(BTN_PIN, INPUT_PULLUP);  

  Serial.begin(9600);
  //while (!Serial);
  
#ifdef DEBUG_ON

  while( Serial.read() != 'S')
  {
    Serial.println("Debug Mode Activé. Presser sur S pour démarrer le programme");
    delay(1000);
  }
  printf_begin();
#endif

  nfc.begin();
  //uint32_t versiondata = nfc.getFirmwareVersion();
  nfc.SAMConfig();
}


uint8_t oldTag[] = {0,0,0,0,0,0,0};

void loop(void) {
  boolean success = false;
  boolean bNew = false;
  uint8_t uid[] = { 
    0, 0, 0, 0, 0, 0, 0                     };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 100);
  if (success) {
    for(int i = 0 ; i < uidLength; i++)
    {
      if(oldTag[i] != uid[i])
      {
        // Le Tag est différent de l'ancien lu
        memcpy( oldTag, uid, 7);
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
          Serial.write(uid[i]);
          //Serial.print(" ");
        }
  #else
        for ( int i = 0; i < ( 7 - uidLength ) ; i++ )
          Serial.write((byte)0x00);
          
        for(int i = 0 ; i < uidLength; i++)
        {
          Serial.write(uid[i]);
          //Serial.print(" ");
        }
       // Serial.println();
  #endif
    }
  }
  else // Il n'y a pas eu de bonne carte 
  {
    memset(oldTag,0,7);
  }
  
  
  // Bouton appuyé
  if(digitalRead(BTN_PIN) == LOW)
  {
    digitalWrite(LED_ROUGE_PIN,LOW);
    if(success)
    {
      memset(oldTag,0,7);
      for(int i = 0 ; i < uidLength; i++)
      {

      }
    }
  }
}
