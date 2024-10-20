#ifndef ULTRASONICRANGE_H
#define ULTRASONICRANGE_H
#include <Arduino.h>

class UltrasonicRange {
  public:		
		  UltrasonicRange(uint8_t trig, uint8_t echo, int rangeCM);
      int getRangeCM();
  private:
		  int status;
      uint8_t PIN_TRIG;
      uint8_t PIN_ECHO;
      uint16_t maxRange;
      uint16_t pulseTO;
};
#endif