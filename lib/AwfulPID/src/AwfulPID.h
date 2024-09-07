#ifndef AWFULPID_H
#define AWFULPID_H
#include <Arduino.h>
#include "Debounce.h"

struct PIDConfiguration{
  uint16_t period_ms; 
  int outMx, outMn;
  bool reverseActing;
};

struct PIDParameters {
  float kp, ki, kd;
};

class AwfulPID {
  public:		
	  AwfulPID();
    void setConfig(PIDConfiguration c);
    void setParam(PIDParameters p);
    void setManual(int mv);
    int update(byte ctrl, int PV, int SP);
    int calculateError();
    int getError();
    int getCV();
  private:
    PIDConfiguration cfg;
    PIDParameters param;
    Debounce cycleTimer;
    int PV, SP, CV, MV;
    int err_last;
    float acc_ki;


};
#endif