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
   Vector v;
   int speed;
};

struct USCommand {
   int pos;
};

class Map2D {
   public:		
      Map2D();
      void setup();
      void update();
      void step();
      void ping(Vector v);      
      void collision();
      DriveCommand getDriveCommand();
      USCommand getUSCommand();
      
      uint8_t data[2500]; //50*50 Grid
   private:
      void setPosGrid(CoordinatesXY c);   
      void nextDriveCmd();
      void nextUSCmd();
      void setMapData(CoordinatesXY c, uint8_t type);
      void pingBuffToGrid(uint8_t type);
      void markEnclosedArea();
      void newMode(uint8_t m);
      void newDriveAction(uint8_t a);
      void newUSAction(uint8_t a);
      void gridSolidify();
      void gridMarkEmpty();
      uint8_t getMapData(CoordinatesXY c, uint8_t fctn, uint8_t* ptrFirstElement);
      CoordinatesXY splitVector(Vector v);
      const int stepSize = 10;
      const int gridSize = 50;
      CoordinatesXY pos, posGrid;
      Vector movement;
      uint8_t mode, modeLast, driveAction, driveAction_last, usAction, usAction_last;
      DriveCommand driveCmd;
      USCommand usCmd;
      Vector pingBuff[36];
      const uint8_t pingBuff_maxSize = 36;
      uint8_t pingBuff_size;
      const float degToRad = 0.017453293; // Pi/180;   
};
#endif