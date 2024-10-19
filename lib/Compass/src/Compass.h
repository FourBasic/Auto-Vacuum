#ifndef COMPASS_H
#define COMPASS_H
#include <Arduino.h>
#include "HMC5883Llib.h"

class Compass {
  public:		
	  Compass();
    int setup();
    int update();
    int get360();
    int get180();
  private:
    Magnetometer mag;
    bool fail;
    int heading360, heading180;
};
#endif