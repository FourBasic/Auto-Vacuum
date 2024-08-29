#ifndef MAP2D_H
#define MAP2D_H
#include <Arduino.h>

struct CoordinatesXY {
   int x,y;
};

struct Vector {
   int dist,dir;
};

class Map2D {
  public:		
	  Map2D();
    void setup();
    int update(Vector ref);
    void setMyPos(CoordinatesXY c);
    CoordinatesXY splitVector(Vector ref);
    uint8_t findQuadIndex(int ref180);
    uint8_t calcDisplacementProposal(CoordinatesXY component, uint8_t index);    
    uint8_t data[2500]; //50*50 Grid
  private:    
    CoordinatesXY myPos;
    CoordinatesXY lastQuadComponent[8];       //Store Last Components at Quadrant Angle Index
    CoordinatesXY displacementProposals[8];
    int displacementProposalsIndex;
};
#endif