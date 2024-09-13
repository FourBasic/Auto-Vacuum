#ifndef AWFULPID_H
#define AWFULPID_H
#include <Arduino.h>
#include "Debounce.h"
#include "GeneralFunctions.h"

struct PIDConfiguration{
  unsigned int period_ms; 
  int outMn;
  int outMx;
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
    int update(byte ctrl, int _PV, int _SP);    
    int getError();
    int getCV();
  private:
    int calculateError();
    PIDConfiguration cfg;
    PIDParameters param;
    Debounce cycleTimer;
    int PV, SP, CV, MV;
    int err_last;
    float acc_ki = 0.0;


};
#endif