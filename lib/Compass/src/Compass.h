#ifndef COMPASS_H
#define COMPASS_H
#include <Arduino.h>
#include "HMC5883Llib.h"

class Compass {
  public:		
	  Compass();
    void setup();
    bool update();
    int convertToHeading180(int heading360);
    int getHeading360();
    int getHeading180();
  private:
    Magnetometer mag;
    bool fail;
    int heading360;
    int heading180;
};
#endif