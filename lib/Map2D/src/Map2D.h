#ifndef MAP2D_H
#define MAP2D_H
#include <Arduino.h>

/* #region DEFINE_GRID_TYPES */
#define GRID_UNKNOWN 0
#define GRID_MYPOS 1
#define GRID_EMPTY 2
#define GRID_SOLID 3
#define GRID_BORDER 4
#define GRID_DIVIDER 5

#define GRID_INVALID 99
/* #endregion */

/* #region DEFINE_MODE_CONTEXT */
#define MODE_BUILD 1
#define MODE_CLEAN 2
/* #endregion */

/* #region DEFINE_OBJECTIVE_CONTEXT */
#define OBJECTIVE_INIT 0
#define OBJECTIVE_CONFIRM_WALLS 1
/* #endregion */

/* #region DEFINE_ACTION_CONTEXT */
// Shared actions (0-19)
#define ACTION_COMPLETE 0
#define ACTION_INIT 1
#define ACTION_WAIT 2
#define ACTION_COLLISION 3
// Ultrasonic actions (20-39)
#define ACTION_US_SWEEP 20
// Drive actions (40-59)
#define ACTION_DRV_GOTO_POS 40
// Tandem actions (60-79)
#define ACTION_TANDEM_WALLRIDE 60
// Map actions (80-99)
#define ACTION_MAP_PING_TO_GRID 80
#define ACTION_MAP_ASSUME_EMPTY 81
#define ACTION_MAP_PATH_TO_NEXT_UNKNOWN 82

/* #endregion */

/* #region DEFINE_COMMAND */
#define CMD_SERVO_HOLD 0
#define CMD_SERVO_GOTO_POS 1
#define CMD_SERVO_GOTO_PING 2
/* #endregion */

/* #region STRUCT */
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
   int function;
   int pos;
};

struct GridDataPoint {
   int elem;
   int type;
};
/* #endregion */

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
      int getGridChangeBuffSize();
      GridDataPoint getGridChangeBuff(int elem);
      void setGridChangeBuffSize(int size);
      uint8_t getObjective();
      uint8_t getMapAction();
      uint8_t getUSAction();
      uint8_t getDriveAction();
      
      uint8_t data[2500]; //50*50 Grid - 30x30cm squares

   private:
      // Objective
      void newMode(uint8_t m);
      void newObjective(uint8_t m);      
      void nextObjective();
      void checkForComplete();
      // Map
      void newMapAction(uint8_t a);      
      void nextMapCmd();      
      void setPosGrid(CoordinatesXY c);
      uint8_t getMapData(CoordinatesXY c);
      void setMapData(CoordinatesXY c, uint8_t type);
      void actMapPingToGrid(uint8_t type);
      void actMapAssumeEmtpy();
      void actMapPathToNearest(uint8_t type);

      void gridSolidify();
      void markEnclosedArea();
      // Ultrasonic 
      void newUSAction(uint8_t a);      
      void nextUSCmd();
      void actUSSweep();
      // Drive
      void newDriveAction(uint8_t a);
      void nextDriveCmd();           
      // Other
      CoordinatesXY splitVector(Vector v);
      void newGridChange(int elem, int type);
      void testGridMon();

      const int stepSize = 10;//10
      const int gridSize = 50;//50
      const int gridSquareSize = 30;//30x30cm Grid Square
      CoordinatesXY pos, posGrid;
      Vector movement;
      uint8_t mode, modeLast, objective, objectiveLast, mapAction, mapAction_last, driveAction, driveAction_last, usAction, usAction_last;
      uint8_t objectiveBuff[10];
      int objectiveBuffIndex;
      DriveCommand driveCmd;
      USCommand usCmd;
      Vector pingBuff[36];
      GridDataPoint gridChangeBuff[100]; 
      int gridChangeBuffSize; 
      const uint8_t pingBuff_maxSize = 36;//36
      uint8_t pingBuff_size;
      const float degToRad = 0.017453293; // Pi/180;
};
#endif