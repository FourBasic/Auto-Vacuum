#include "Map2D.h"
#include <Arduino.h>
#include "GeneralFunctions.h"

#define MODE_BUILD 1
#define MODE_CLEAN 2

#define INIT 1
#define COLLISION 2
#define CMD_COMPLETE 3
#define WALLRIDE 4
#define OPENAREA 5

Map2D::Map2D() { }

void Map2D::setup() {
    CoordinatesXY c;
    c.x = 0;
    c.y = 0;
    setPosGrid(c);
    // Check EEPROM if map exists and load into memory, otherwise build
    // Just build for initial testing
    nextCmd(MODE_BUILD, INIT);
}

// Counts steps in a given direction
void Map2D::step() {
    movement.dist = movement.dist + stepSize;
    if (movement.dist >= cmd.dist) {
        nextCmd(lastMode, CMD_COMPLETE);
    }
}

Vector Map2D::nextCmd(int mode, int context) {
    lastMode = mode;
    lastContext = context;
    Vector v;
    return v;
}

void Map2D::setPosGrid(CoordinatesXY c) {
    posGrid.x = c.x;
    posGrid.y = c.y;
    pos.x = posGrid.x * stepSize;
    pos.y = posGrid.y * stepSize;
}

CoordinatesXY Map2D::splitVector(Vector v) {
    CoordinatesXY c;
    c.x = sin(v.dir) * v.dist;
    c.y = cos(v.dir) * v.dist;
    return c; 
}





