#ifndef COMPASS_H
#define COMPASS_H
#include <Arduino.h>

class Compass {
  public:		
		Compass(uint8_t PIN);
  private:
		int status;
    int8_t PIN_;
};
#endif