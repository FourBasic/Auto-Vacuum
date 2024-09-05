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
      void setPosGrid(CoordinatesXY c);
      void step();
      Vector nextCmd(int mode, int context);      
      CoordinatesXY splitVector(Vector v);
      uint8_t data[2500]; //50*50 Grid
   private:
      const int stepSize = 10;
      const int gridSize = 50;
      CoordinatesXY pos;
      CoordinatesXY posGrid;
      Vector movement;
      Vector cmd;
      int lastContext;
      int lastMode;
};
#endif