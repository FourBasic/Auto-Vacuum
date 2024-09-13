#include "IOSimulate.h"
#include <Arduino.h>
#include "Debounce.h"

#define MODE_SWITCH 0
#define MODE_PULSE 1
#define MODE_ANALOG 2


IOSimulate::IOSimulate() { }

void IOSimulate::setup() {
    if (!Serial) { Serial.begin(9600); }
    for (int i=0; i<10; i++) { pin[i].pinClock.setup(0); }
}

// Deal with time based pin functions (such as pulse)
void IOSimulate::update() {
    for (int i=0; i<10; i++) {        
        if (pin[i].mode == MODE_PULSE) {
            pin[i].state = (int) pin[i].pinClock.update(!pin[i].pinClock.getState(), pin[i].onT, pin[i].offT);
        }
    }
}

// 0-switch | 1-Pulse | 2-Analog
void IOSimulate::setPinMode(int _pin, uint8_t _mode) {
    pin[_pin].mode = _mode;
}

// Comment
void IOSimulate::setPinState(int _pin, int _state) {
    pin[_pin].state = _state;
}

void IOSimulate::setPinClock(int _pin, unsigned int _onT, unsigned int _offT) {
    pin[_pin].onT = _onT;
    pin[_pin].offT = _offT;
}

// Comment
int IOSimulate::getPinState(int _pin) {
    return pin[_pin].state;
}


