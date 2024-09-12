#include "IODriver.h"
#include <Arduino.h>
#include "Debounce.h"

#define MODE_SWITCH 0
#define MODE_PULSE 1
#define MODE_ANALOG 2


IODriver::IODriver() { }

void IODriver::setup() {
    if (!Serial) { Serial.begin(9600); }
    for (int i=0; i<10; i++) { pin[i].pinClock.setup(0); }
}

// Deal with time based pin functions (such as pulse)
void IODriver::update() {
    for (int i=0; i<10; i++) {        
        if (pin[i].mode == MODE_PULSE) {
            pin[i].state = (int) pin[i].pinClock.update(!pin[i].pinClock.getState(), pin[i].onT, pin[i].offT);
        }
    }
}

// 0-switch | 1-Pulse | 2-Analog
void IODriver::setPinMode(int _pin, uint8_t _mode) {
    pin[_pin].mode = _mode;
}

// Comment
void IODriver::setPinState(int _pin, int _state) {
    pin[_pin].state = _state;
}

void IODriver::setPinClock(int _pin, unsigned int _onT, unsigned int _offT) {
    pin[_pin].onT = _onT;
    pin[_pin].offT = _offT;
}

// Comment
int IODriver::getPinState(int _pin) {
    return pin[_pin].state;
}


