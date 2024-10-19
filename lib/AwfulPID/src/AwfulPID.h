#ifndef AWFULPID_H
#define AWFULPID_H
#include <Arduino.h>
#include "Debounce.h"
#include "GeneralFunctions.h"

#define PID_INIT        0x00
#define PID_ENABLE      0x01
#define PID_TIEBACK     0x02
#define PID_MANUAL      0x03

// period_ms / outMn / outMx / reverseActing / stableTol / stablePeriodCount
struct PIDConfiguration{
  unsigned int period_ms;
  int outMn;
  int outMx;
  bool reverseActing;
  int stableTol;
  int stablePeriodCount;
};

// kp / ki / kd
struct PIDParameters {
  float kp, ki, kd;
};

class AwfulPID {
  public:		
	  AwfulPID();
    void setConfig(PIDConfiguration c);
    void setParam(PIDParameters p);
    void setManual(int mv);
    int update(byte ctrl, int _PV, int _SP);    
    int getError();
    int getCV();
    bool getStability();
  private:
    int calculateError();
    PIDConfiguration cfg;
    PIDParameters param;
    Debounce cycleTimer;
    int PV, SP, CV, MV;
    int err_last;
    float acc_ki = 0.0;
    bool stable = false;
    int stableCycles = 0;

};
#endif