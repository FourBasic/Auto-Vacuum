#ifndef MAP2D_H
#define MAP2D_H
#include <Arduino.h>

struct CoordinatesXY {
   int x,y;
};

struct Vector {
   int dist,dir;
};

struct DriveCommand {
   Vector dest;
   int speed;
};

struct USCommand {
   int pos;
};

class Map2D {
   public:		
      Map2D();
      void setup();
      void setPosGrid(CoordinatesXY c);
      void step();
      void ping(Vector v);
      void update();
      void collision();
      DriveCommand getDriveCommand();
      USCommand getUSCommand();
      
      uint8_t data[2500]; //50*50 Grid
   private:
      void nextDriveCmd();
      void nextUSCmd();
      CoordinatesXY splitVector(Vector v);
      const int stepSize = 10;
      const int gridSize = 50;
      CoordinatesXY pos;
      CoordinatesXY posGrid;
      Vector movement;
      int mode;
      int driveAction;
      int usAction;
      DriveCommand driveCmd;
      USCommand usCmd;
};
#endif