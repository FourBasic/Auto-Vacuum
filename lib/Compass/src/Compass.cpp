#include "Compass.h"
#include <Arduino.h>
#include "HMC5883Llib.h"

Compass::Compass() { }

int Compass::setup() {
    if (!Serial) { Serial.begin(9600); }

    if (mag.begin() != 0) {
        Serial.println("Error connecting to Magnetometer");
        fail = true;
        return -1;
    }
    // MSB/Gauss   Field Range
    // 1370     +- 0.88 Ga
    // 1090     +- 1.3 Ga
    // 820      +- 1.9 Ga
    // 660      +- 2.5 Ga
    // 440      +- 4.0 Ga
    // 390      +- 4.7 Ga
    // 330      +- 5.6 Ga
    // 230      +- 8.1 Ga
    mag.setGain(HMC5833L_GAIN_1370);
    return 0;
}

int Compass::update() {
    // don't bother reading if we failed to connect
    if (fail)
        return -1;
    double heading;
    //int16_t x;
    //int16_t y;
    //int16_t z;
    // reads the heading in degrees using the X and Y axis
    int8_t ret = mag.readHeadingDeg(&heading);    
    //int8_t ret = mag.readRaw(&x,&y,&z);    
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
    switch (ret) {
        case HMC5833L_ERROR_GAINOVERFLOW:
            Serial.println("Gain Overflow");
            return -1;
        case 0:
            break;
        default:
            Serial.println("Failed to read Magnetometer");
            return -1;
    }
    
    //Serial.print("Heading: ");
    //Serial.print(heading);
    //Serial.println(" degrees");
    heading360 = heading;
    if (heading360 < 180) { heading180 = heading360; }
    else { heading180 = heading360 - 360; }
    return heading;
}

int Compass::get360() {
    return heading360;
}

//  +-180
int Compass::get180() {
    return heading180;
}