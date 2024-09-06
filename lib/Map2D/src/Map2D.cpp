#include "Map2D.h"
#include <Arduino.h>
#include "GeneralFunctions.h"

#define MODE_BUILD 1
#define MODE_CLEAN 2
#define INIT 1
#define COLLISION 2
#define COMPLETE 3
#define WALLRIDE 4
#define OPENAREA 5
#define US_SWEEP 6
#define CRUISE 7
#define WAIT 8

Map2D::Map2D() { }
/* INTERFACE FUNCTIONS */
/* --------------------*/
// Externally called - Controller boot
void Map2D::setup() {
    CoordinatesXY c;
    c.x = 0;
    c.y = 0;
    setPosGrid(c);
    // Check EEPROM if map exists and load into memory, otherwise build
    // Just build for initial testing
    mode = MODE_BUILD;
    driveAction = INIT;
    usAction = INIT;
    nextDriveCmd();
    nextUSCmd();
}

// Externally called - counts drive steps to flag drive action completion
void Map2D::step() {
    movement.dist = movement.dist + stepSize;
    if (movement.dist >= driveCmd.dest.dist) {
        driveAction = COMPLETE;
    }
}

// Externally called - somehow pings to flag usAction completion
void Map2D::ping(Vector v) {
    // if sweep then populate wall data
    // else compare with map expected values and issue corrections
    if (v.dir == 360) {
        usAction = COMPLETE;
    }
}

// Externally called - unexpected event, need to recalculate
void Map2D::collision() {
    driveAction = COLLISION;
    usAction = COLLISION;
    nextDriveCmd();
    nextUSCmd();
}

// Externally called - Tells drive what it should be doing
DriveCommand Map2D::getDriveCommand() {
    return driveCmd;
}

// Externally called - Tells US what it should be doing
USCommand Map2D::getUSCommand() {
    return usCmd;
}

/* CORE FUNCTIONS */
/* --------------------*/
// Issue the next drive command
void Map2D::nextDriveCmd() {
    if (mode == MODE_BUILD) {
        if (driveAction == INIT) {
            driveAction = WAIT;
        }
    }
}

// Issue the next ultrasonic command
void Map2D::nextUSCmd() {
    if (mode == MODE_BUILD) {
        if (usAction == INIT) {
            usAction = US_SWEEP;
        }
    }
}

// Something to call internal timers and other periodic checks
void Map2D::update() {
    // timers

    // call next command when wait condition is satisfied
    if (driveAction == WAIT && usAction == COMPLETE) {
        nextDriveCmd();
    }
    if (usAction == WAIT && driveAction == COMPLETE) {
        nextUSCmd();
    }

}

/* ASSISTING FUNCTIONS */
/* --------------------*/
// Set the current coordinates
void Map2D::setPosGrid(CoordinatesXY c) {
    posGrid.x = c.x;
    posGrid.y = c.y;
    pos.x = posGrid.x * stepSize;
    pos.y = posGrid.y * stepSize;
}

// Split Hypotenuse
CoordinatesXY Map2D::splitVector(Vector v) {
    CoordinatesXY c;
    c.x = sin(v.dir) * v.dist;
    c.y = cos(v.dir) * v.dist;
    return c; 
}





