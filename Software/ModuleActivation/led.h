

#ifndef __LED__h__
#define __LED__h__

#include <Arduino.h>
#define SPEED_FAST 100
#define SPEED_SLOW 500
class led{
public:
  /******** Constructeurs ********/
  /**
   * \pin: Nom de la pin de la led
   **/
  led(int pinR,int pinV);


  /******** Fonction public ********/
  void init();

  void setFastR();
  void setFastV();
  void setSlowR();
  void setSlowV();
  void setOnV();
  void setOnR();
  void setOffV();
  void setOffR();
  void run();
private:
  char pinLedR,pinLedV, speedR,speedV;
  unsigned long lastRunFast,lastRunSlow;              // Dernier appel
  unsigned char lastStateFast,lastStateSlow;

};
#endif

