#include <Arduino.h>
#include "led.h"

led::led(int pinR,int pinV){
  pinMode(pinR,OUTPUT);
  pinMode(pinV,OUTPUT);
  pinLedR = pinR;
  pinLedV = pinV;
  
  init();
}
void led::init(){
  lastRunFast = 0;
  lastRunSlow = 0;
  speedR =0;
  speedV =0;
  digitalWrite(pinLedV,LOW);
  digitalWrite(pinLedR,LOW);
  lastStateFast=0;
  lastStateSlow=0;
}
void led::setFastR(){
  speedR = 2;
}
void led::setFastV(){
  speedV = 2;
}
void led::setSlowR(){
  speedR = 1;
}
void led::setSlowV(){
  speedV = 1;
}
void led::setOnV(){
  speedV = 0;
  digitalWrite(pinLedV,HIGH);
}
void led::setOnR(){
  speedR = 0;
  digitalWrite(pinLedR,HIGH);
}
void led::setOffV(){
  speedV = 0;
  digitalWrite(pinLedV,LOW);
}
void led::setOffR(){
  speedR = 0;
  digitalWrite(pinLedR,LOW);
}
void led::run(){ 
  long curr = millis();
  if( curr - lastRunFast >= SPEED_FAST)
  {
    lastRunFast += SPEED_FAST;
     lastStateFast = !lastStateFast;
    if(speedR == 2)
        digitalWrite(pinLedR,lastStateFast);
    if(speedV == 2)
        digitalWrite(pinLedV,lastStateFast);
  }
  if( curr - lastRunSlow >= SPEED_SLOW)
  {
    lastRunSlow += SPEED_SLOW;
    lastStateSlow = !lastStateSlow;
    if(speedR == 1)
        digitalWrite(pinLedR,lastStateSlow);
    if(speedV == 1)
        digitalWrite(pinLedV,lastStateSlow);
  } 
}
  
