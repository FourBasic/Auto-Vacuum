#ifndef COMPASS_H
#define COMPASS_H
#include <Arduino.h>
#include "HMC5883Llib.h"

class Compass {
  public:		
	  Compass();
    int setup();
    int getHeading();
  private:
    Magnetometer mag;
    bool fail;
};
#endif