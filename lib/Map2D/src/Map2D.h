#ifndef MAP2D_H
#define MAP2D_H
#include <Arduino.h>

/* #region DEFINE_GRID_TYPES */
#define GRID_UNKNOWN 0
#define GRID_SOLID 1
#define GRID_BORDER 2
#define GRID_DIVIDER 3
#define GRID_EMPTY 4
#define GRID_MYPOS 2
/* #endregion */

/* #region DEFINE_ACTION_CONTEXT */
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
/* #endregion */

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
   //int function;
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
      
      uint8_t data[2500]; //50*50 Grid - 30x30cm squares
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
      const int stepSize = 10;//10
      const int gridSize = 50;//50
      const int gridSquareSize = 30;//30x30cm Grid Square
      CoordinatesXY pos, posGrid;
      Vector movement;
      uint8_t mode, modeLast, driveAction, driveAction_last, usAction, usAction_last;
      DriveCommand driveCmd;
      USCommand usCmd;
      Vector pingBuff[36];
      const uint8_t pingBuff_maxSize = 36;//36
      uint8_t pingBuff_size;
      const float degToRad = 0.017453293; // Pi/180;
};
#endif