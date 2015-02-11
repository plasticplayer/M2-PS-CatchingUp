

#ifndef __LED__h__
#define __LED__h__

#include <Arduino.h>

class led{
   public:
      /******** Constructeurs ********/
      /**
        \pin: Nom de la pin de la led
      **/
      led(int pin);
      
      
      /******** Fonction public ********/
      void init();
       /**
        * Defini le temps entre chaque changement d etat
        \time: Duree en millisecondes. 0 ==> pas de changement
      **/
      void speed(int time);
       /**
        * Switch l etat de la led si le delais est depasse
      **/
      void run();
       /**
        * Allume la led
      **/
      void on();
       /**
        * Eteint la led
      **/
      void off();
       /**
        Inverse l etat de la led
      **/
      void toggle();
   private:
     int pinLed, timeLed,ledLastValue;
     unsigned long lastCall;              // Dernier appel

};
#endif
