#include <Arduino.h>
#include "led.h"

led::led(int pin){
  pinMode(pin,OUTPUT);
  pinLed = pin;
}
void led::init(){
  timeLed = 0;
  off();
}

void led::speed(int time){
  timeLed = time;
}

void led::run(){
  
  if ( timeLed == 0 ){
      //if ( ledLastValue != 0 )
      //  off();
        
     return;
  }
 
  long diff = millis() - lastCall;
  if ( diff >= timeLed ) {
    toggle();
  }
  
}
  
 
void led::on(){
  digitalWrite(pinLed,HIGH);
  ledLastValue = 1;
  lastCall = millis();
}
void led::off(){
  digitalWrite(pinLed,LOW);
  ledLastValue = 0;
  lastCall = millis();
}
void led::toggle(){
  if ( ledLastValue == 1)
    off();
  else
    on();
}
