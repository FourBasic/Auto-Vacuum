#include "Map2D.h"
#include <Arduino.h>
#include "GeneralFunctions.h"

Map2D::Map2D() { }

void Map2D::setup() {
    myPos.x = 0; myPos.y = 0;
}

void Map2D::update(int heading) {
    heading180 = heading;
}

// Uses vector as a sample to determine relative movement from last known absolute position.
// This is wrong. Need x2 us pointing front and rear. Only reliable dist. components still calc'd w.compass
int Map2D::updatePosition(Vector vf, Vector vr) {
    // break reference into its components
    CoordinatesXY componentF;
    componentF = splitVector(vf);

    // previous components@angle are stored to determine displacement by differentiating
    // find the index contataining components matching direction of argument V.
    uint8_t quadIndex;
    quadIndex = findQuadIndex(vf.dir);

    // Differentiate/determine displacement and reject impossible results
    // Add valid results to proposal buffer to later be used in estimating actual displacement
    uint8_t proposalAdded;
    if (quadIndex > -1 && quadIndex < 9) {
        proposalAdded = calcDisplacementProposal(componentF, quadIndex);
    }
}

// Set current absolute position
void Map2D::setMyPos(CoordinatesXY c) {
    myPos = c;
    // Flush all proposals?? Dont think it is needed. Just shifting frame of reference, disp still valid.
    //displacementProposalsIndex = 0;
}

// 
uint8_t* Map2D::getDataBlock() {
    uint8_t* ptr; 
    ptr = &data[0];
    return ptr;
}

// Set a map data array element - index<2500 
void Map2D::setDataPoint(int index, uint8_t val) {
    data[index] = val;
}

// Find the quadrant index with matching angle 99=error 
uint8_t Map2D::findQuadIndex(int ref180) {
    // convert +-180 angle to 360
    int ref360;
    if (ref180 >= 0) { ref360 = ref180; } else { ref360 = 360 + ref180; }

    // Find the quadrant index with closest matching angle
    uint8_t quadIndex;
    for (int quad=0; quad<360; quad+=45) {
        if (withinRange(quad, ref360, 1)) { return quadIndex; break; }
        quadIndex++;
    }
    return 99;
}

// Derives XY components from vector
CoordinatesXY Map2D::splitVector(Vector v) {
    CoordinatesXY c;
    c.x = sin(v.dir) * v.dist;
    c.y = cos(v.dir) * v.dist;
    return c; 
}

// Differentiate between new and old component-add to proposal buffer
uint8_t Map2D::calcDisplacementProposal(CoordinatesXY component, uint8_t index) {
    CoordinatesXY disp;
    disp.x = lastQuadComponent[index].x - component.x;
    disp.y = lastQuadComponent[index].y - component.y;
    
    // Probably want some error checking and maybe add time to the structure
    // to get velocity to check validity of data against a maxVelocity
    bool reject;
    reject = reject; //...eventually
    if (!reject) {
        // Replace old components with new
        lastQuadComponent[index].x = component.x;
        lastQuadComponent[index].y = component.y;
        // Add to proposal buffer
        displacementProposals[displacementProposalsIndex] = disp;
        displacementProposalsIndex ++;
        return 1;
    } else { return 0; }
}

