#include "Map2D.h"
#include <Arduino.h>
//#include "GeneralFunctions.h"

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

#define GRID_UNKNOWN 0
#define GRID_SOLID 1
#define GRID_BORDER 2
#define GRID_DIVIDER 3
#define GRID_EMPTY 4

#define XFER_SINGLE 0
#define XFER_PERIM_ONE 1

Map2D::Map2D() { }

/* INTERFACE FUNCTIONS */
/* ------------------------------------------------------------ */
// Externally called - Controller boot
void Map2D::setup() {
    CoordinatesXY c;
    c.x = 0;
    c.y = 0;
    setPosGrid(c);
    // Check EEPROM if map exists and load into memory, otherwise build
    // Just build for initial testing
    newMode(MODE_BUILD);
    newDriveAction(INIT);
    newUSAction(INIT);
    nextDriveCmd();
    nextUSCmd();
}

// Externally called - counts drive steps to flag drive action completion
void Map2D::step() {
    movement.dist = movement.dist + stepSize;
    if (movement.dist >= driveCmd.v.dist) {
        driveAction = COMPLETE;
    }
}

// Externally called - somehow pings to flag usAction completion
void Map2D::ping(Vector v) {
    // Populate ping buffer --> Buffer analyzed in other functions
    if (pingBuff_size != pingBuff_maxSize) {
        pingBuff[pingBuff_size] = v;
        pingBuff_size ++;
    }
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
/* ------------------------------------------------------------ */

// Something to call internal timers and other periodic checks
void Map2D::update() {
    // timers?

    // Set complete when wait condition is satisfied
    if (driveAction == WAIT && usAction == COMPLETE) { newDriveAction(COMPLETE); }
    if (usAction == WAIT && driveAction == COMPLETE) { newUSAction(COMPLETE); }

    // Generate next command on complete
    if (driveAction == COMPLETE) { nextDriveCmd(); }
    if (usAction == COMPLETE) { nextUSCmd(); }

}

// Determine the next drive command
void Map2D::nextDriveCmd() {
    if (mode == MODE_BUILD) {
        if (driveAction == INIT) {
            newDriveAction(WAIT);
            driveCmd.speed = 0;
        } else if (driveAction == COMPLETE) {
            pingBuffToGrid(GRID_SOLID);
            gridSolidify();            
            gridMarkEmpty();
            driveCmd.speed = 255;
            driveCmd.v.dir = 50;
            driveCmd.v.dist = 50;
        }
    }
}

// Determine the next ultrasonic command
void Map2D::nextUSCmd() {
    if (mode == MODE_BUILD) {
        if (usAction == INIT) {
            newUSAction(US_SWEEP);
        }
    }
}


/* MAP BUILD FUNCTIONS */
/* ------------------------------------------------------------ */

// Deduce grid area that is empty
void Map2D::markEnclosedArea() {

}

// Convert ping buff relative v to grid absolute XY
void Map2D::pingBuffToGrid(uint8_t type) {
    for (int i=0; i<pingBuff_size; i++) {
        // Break the ping buffer data into XY components
        CoordinatesXY pingComponent = splitVector(pingBuff[i]);
        
        // Determine the grid position of the ping by adding components to current position
        CoordinatesXY gridCoord;
        gridCoord.x = pingComponent.x + posGrid.x;
        gridCoord.y = pingComponent.y + posGrid.y;

        // Write type to grid position
        setMapData(gridCoord, GRID_SOLID);
    }
}

// Find and fill in small gaps in near continuous solids
void Map2D::gridSolidify() {
    // Search around solid 1x1 square
    // Check square radius of 3x3 for an unknown
    // Check square radius of 5x5 for another solid
    // Stuff get complicated when considering what has already been checked? 
    // Should be fine, whole blast radius will be "Checked"
    // Add a modifier to read/write map data in chunks more efficiently. 
    // ie getSurrounding(radius) Def use pointers so not array size dependant
    CoordinatesXY checkCoord;
    uint8_t perimData[3];
    getMapData(checkCoord, XFER_PERIM_ONE, &perimData[0]);
}

// Mark deduced grid empty spaces
void Map2D::gridMarkEmpty() {

}


/* ASSISTING FUNCTIONS */
/* ------------------------------------------------------------ */

// Set the current coordinates
void Map2D::setPosGrid(CoordinatesXY c) {
    posGrid.x = c.x;
    posGrid.y = c.y;
    pos.x = posGrid.x * stepSize;
    pos.y = posGrid.y * stepSize;
}

// Split Hypotenuse
CoordinatesXY Map2D::splitVector(Vector v) {
    // Generally working in Deg b/c it is more practical for integers
    // Arduino trig functions take Rad so...
    v.dir = v.dir * degToRad;

    // Find components
    CoordinatesXY c;
    c.x = sin(v.dir) * v.dist;
    c.y = cos(v.dir) * v.dist;
    return c; 
}

// Update current mode and last mode
void Map2D::newMode(uint8_t m) {
    modeLast = mode;
    mode = m;
}

// Update current action and last action
void Map2D::newDriveAction(uint8_t a) {
    driveAction_last = driveAction;
    driveAction = a;
}

// Update current action and last action
void Map2D::newUSAction(uint8_t a) {
    usAction_last = usAction;
    usAction = a;
}

// Inserts grid square type in data array at given XY 
void Map2D::setMapData(CoordinatesXY c, uint8_t type) {
    int i = (c.x * gridSize) + c.y;
    data[i] = type;
}

// Returns grid square type from data array at given XY
uint8_t Map2D::getMapData(CoordinatesXY c, uint8_t fctn, uint8_t* ptrFirstElement) {
    
    // remove return from function even for single
    uint8_t actualData[5]; //delete
    if (fctn == XFER_SINGLE) { 
        int i = (c.x * gridSize) + c.y;
        return data[i];
    } else if (fctn == XFER_PERIM_ONE) {
        // Is this even legal or remotely correct?
        ptrFirstElement ++; // Index memory address (Presumably by 1 byte)
        *ptrFirstElement = actualData[2]; // Set that second byte address to a value?
    }


    
}