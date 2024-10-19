/* #region INCLUDE */
#include "Map2D.h"
#include <Arduino.h>
#include "GeneralFunctions.h"
/* #endregion */

/* #region DEFINE_FUNCTION_MODIFIER */
#define XFER_SINGLE 0
#define XFER_PERIM_ONE 1
/* #endregion */

Map2D::Map2D() { }

/* #region EXTERNAL_CALL */

// Initialize & retrieve map from EEPROM
void Map2D::setup() {
    CoordinatesXY c;
    c.x = 24;
    c.y = 24;
    setPosGrid(c);
    pingBuff_size = 0;
    usCmd.pos = 0;
    usCmd.function = CMD_SERVO_GOTO_POS;
    // Check EEPROM if map exists and load into memory, otherwise build
    // Just build for initial testing
    newMode(MODE_BUILD);
    newDriveAction(ACTION_INIT);
    newUSAction(ACTION_INIT);
    nextDriveCmd();
    nextUSCmd();
}

// Something to call internal timers and other periodic checks
void Map2D::update() {
    // timers?

    // Set complete when wait condition is satisfied
    if (driveAction == ACTION_WAIT && usAction == ACTION_COMPLETE) { newDriveAction(ACTION_COMPLETE); }
    if (usAction == ACTION_WAIT && driveAction == ACTION_COMPLETE) { newUSAction(ACTION_COMPLETE); }

    // Generate next command on complete
    if (driveAction == ACTION_COMPLETE) { nextDriveCmd(); }
    if (usAction == ACTION_COMPLETE) { nextUSCmd(); }

    // Completion conditions for sweep
    if (usAction == ACTION_US_SWEEP) {
        if (pingBuff_size < 4) {
            usCmd.pos = (pingBuff_size * 45)+90;
        } else {
            usCmd.function = CMD_SERVO_GOTO_POS;
            usCmd.pos = 0;
            pingBuffToGrid(GRID_SOLID);
            //gridSolidify();            
            //gridMarkEmpty();
            newUSAction(ACTION_COMPLETE);
        }
    }

}

// Counts drive steps to flag drive action completion
void Map2D::step() {
    movement.dist = movement.dist + stepSize;
    if (movement.dist >= driveCmd.v.dist) {
        driveAction = ACTION_COMPLETE;
    }
}

// Somehow pings to flag usAction completion
void Map2D::ping(Vector v) {
    // Populate ping buffer --> Buffer analyzed in other functions
    if (pingBuff_size != pingBuff_maxSize) {
        //itshere;
        pingBuff[pingBuff_size] = v;
        pingBuff_size ++;
        Serial.print("dir ");
        Serial.print(v.dir);
        Serial.print(" - dist ");
        Serial.println(v.dist);
    } else { Serial.println("ping reject"); }
    // else compare with map expected values and issue corrections
    //if (v.dir == 360) {
        //usAction = COMPLETE;
    //}
}

// Unexpected event, need to recalculate
void Map2D::collision() {
    driveAction = ACTION_COLLISION;
    usAction = ACTION_COLLISION;
    nextDriveCmd();
    nextUSCmd();
}

// Get current drive objective
DriveCommand Map2D::getDriveCommand() {
    return driveCmd;
}

// Get current ultrasonic objective
USCommand Map2D::getUSCommand() {
    return usCmd;
}

/* #endregion */

/* #region CORE */

// Determine the next drive command
void Map2D::nextDriveCmd() {
    if (mode == MODE_BUILD) {
        if (driveAction == ACTION_INIT) {
            newDriveAction(ACTION_WAIT);
            driveCmd.speed = 0;
        } else if (driveAction == ACTION_COMPLETE) {            
            driveCmd.speed = 255;
            driveCmd.v.dir = 50;
            driveCmd.v.dist = 50;
        }
    }
}

// Determine the next ultrasonic command
void Map2D::nextUSCmd() {
    if (mode == MODE_BUILD) {
        if (usAction == ACTION_INIT) {
            newUSAction(ACTION_US_SWEEP);
            usCmd.function = CMD_SERVO_GOTO_PING;
            usCmd.pos = (pingBuff_size * 45)+90;
        }
    }
}

/* #endregion */

/* #region MAP_BUILD */

// Deduce grid area that is empty
void Map2D::markEnclosedArea() {

}

// Convert ping buff relative v to grid absolute XY
void Map2D::pingBuffToGrid(uint8_t type) {    
    for (int i=0; i<pingBuff_size; i++) {
        // Skip out of range pings (-1)
        if (pingBuff[i].dist != -1) {
            // Break the ping buffer data into XY components
            CoordinatesXY pingComponent = splitVector(pingBuff[i]);

            // Scale vectors to gridaquare size
            pingComponent.x = pingComponent.x / gridSquareSize;
            pingComponent.y = pingComponent.y / gridSquareSize;

            // Determine the grid position of the ping by adding components to current position
            CoordinatesXY gridCoord;
            gridCoord.x = pingComponent.x + posGrid.x;
            gridCoord.y = pingComponent.y + posGrid.y;            

            // Limit to array size
            // Need to deal with negatives by shifting map!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            gridCoord.x = limit(0, gridCoord.x, gridSize);
            gridCoord.y = limit(0, gridCoord.y, gridSize);

            Serial.print("coordX=");
            Serial.print(gridCoord.x);
            Serial.print(" - coordY=");
            Serial.println(gridCoord.y);

            // Write type to grid position
            setMapData(gridCoord, GRID_SOLID);
            //i = 99;
        } else {Serial.print("O/R");}
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

/* #endregion */

/* #region ASSISTING */

// Set the current coordinates
void Map2D::setPosGrid(CoordinatesXY c) {
    posGrid.x = c.x;
    posGrid.y = c.y;
    pos.x = posGrid.x * gridSquareSize;
    pos.y = posGrid.y * gridSquareSize;
    setMapData(posGrid, GRID_MYPOS);
}

// Split Hypotenuse
CoordinatesXY Map2D::splitVector(Vector v) {
    // Generally working in Deg b/c it is more practical for integers
    // Arduino trig functions take Rad so...
    float dirRad;
    dirRad = v.dir * degToRad;

    // Find components
    CoordinatesXY c;
    c.x = sin(dirRad) * v.dist;
    c.y = cos(dirRad) * v.dist;
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

/* #endregion */