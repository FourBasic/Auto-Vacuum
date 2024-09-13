#ifndef IOSIMULATE_H
#define IOSIMULATE_H
#include <Arduino.h>
#include "Debounce.h"

struct PinData {
    int state;
    uint8_t mode;
    Debounce pinClock;
    unsigned int onT, offT;
};

class IOSimulate {
  public:		
    IOSimulate();
    void setup();
    void update();
    void setPinMode(int _pin, uint8_t _mode);
    void setPinState(int _pin, int _state);
    void setPinClock(int _pin, unsigned int _onT, unsigned int _offT);
    int getPinState(int _pin);
  private:
    PinData pin[10];
};
#endif