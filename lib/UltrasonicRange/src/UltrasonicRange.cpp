#include "UltrasonicRange.h"
#include <Arduino.h>

// Speed of Sound 340 m/s = 0.034 cm/us
// distance = travelDistance / 2 = 0.034 * pulseDuration / 2 = 0.017

UltrasonicRange::UltrasonicRange(uint8_t trig, uint8_t echo, int rangeCM) {
    PIN_TRIG = trig;
    PIN_ECHO = echo;
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    digitalWrite(PIN_TRIG, false);
    maxRange = rangeCM;
    pulseTO = maxRange / 0.017;
}

uint16_t UltrasonicRange::getRangeCM() {
    // generate 10-microsecond pulse to TRIG pin
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    // measure duration of pulse from ECHO pin
    int duration;
    duration = pulseIn(PIN_ECHO, HIGH, pulseTO);

    // calculate the distance - Will return 0 when out of range
    uint16_t dist = 0;
    if (!duration) {
        Serial.println("O/R");
        return dist;
    } else {
        dist = 0.017 * duration;
        Serial.print(dist);
        Serial.println(" cm");
        return dist;
    }
}

