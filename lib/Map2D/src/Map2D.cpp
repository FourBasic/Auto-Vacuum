/* #region INCLUDE */
#include "Map2D.h"
#include <Arduino.h>
#include "GeneralFunctions.h"
/* #endregion */

/***********************************************************************************************************************************************/

/* #region DEFINE_FUNCTION_MODIFIER */
#define XFER_SINGLE 0
#define XFER_PERIM_ONE 1
/* #endregion */

/***********************************************************************************************************************************************/

Map2D::Map2D() { }

/***********************************************************************************************************************************************/

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
        newObjective(OBJECTIVE_CONFIRM_WALLS);
        newDriveAction(ACTION_INIT);
        newUSAction(ACTION_INIT);
        nextDriveCmd();
        nextUSCmd();
    }

    // Call current action functions
    void Map2D::update() {

        // Set complete when wait condition is satisfied
        if (driveAction == ACTION_WAIT && usAction == ACTION_COMPLETE) { newDriveAction(ACTION_COMPLETE); }
        if (usAction == ACTION_WAIT && driveAction == ACTION_COMPLETE) { newUSAction(ACTION_COMPLETE); }

        // Index objective buffer for next action 
        if (driveAction == ACTION_COMPLETE && usAction == ACTION_COMPLETE) {
            // Mark current as completed and load next
            objectiveBuff[objectiveBuffIndex] = ACTION_COMPLETE;
            objectiveBuffIndex += 1;
            // Load next objective if current buffer is complete
            if (objectiveBuff[objectiveBuffIndex] == ACTION_COMPLETE) { nextObjective(); }   
            // Load next device commands based on current buffer action             
            nextDriveCmd();
            nextUSCmd();
        }

        // Call any active device action functions
        if (usAction == ACTION_US_SWEEP) { actionSweep(); }
    }

    // Counts drive steps to flag drive action completion
    void Map2D::step() {
        movement.dist = movement.dist + stepSize;
        if (movement.dist >= driveCmd.v.dist) {
            driveAction = ACTION_COMPLETE;
        }
    }

    // Add US ping vector to a buffer to be analyzed later
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

    // Unexpected event, need to reroute
    void Map2D::collision() {
        driveAction = ACTION_COLLISION;
        usAction = ACTION_COLLISION;
        nextDriveCmd();
        nextUSCmd();
    }

    // Get current drive task
    DriveCommand Map2D::getDriveCommand() {
        return driveCmd;
    }

    // Get current ultrasonic task
    USCommand Map2D::getUSCommand() {
        return usCmd;
    }

/* #endregion */

/***********************************************************************************************************************************************/

/* #region OBJECTIVE */

    // Update current mode and last mode
    void Map2D::newMode(uint8_t m) {
        modeLast = mode;
        mode = m;
    }

        // Update current mode and last mode
    void Map2D::newObjective(uint8_t o) {
        objectiveLast = objective;
        objective = o;
    }

    void Map2D::nextObjective(){
        if (mode == MODE_BUILD) {
            // First build sequence. Find all walls until entire map is enclosed (ACTION_MAP_PATH_TO_NEXT_UNKNOWN fails)
            if (objective == OBJECTIVE_CONFIRM_WALLS) {
                objectiveBuffIndex = 0;
                objectiveBuff[0] = ACTION_US_SWEEP;
                objectiveBuff[1] = ACTION_MAP_PING_TO_GRID;
                objectiveBuff[2] = ACTION_MAP_ASSUME_EMPTY;
                objectiveBuff[3] = ACTION_MAP_PATH_TO_NEXT_UNKNOWN;
                objectiveBuff[4] = ACTION_DRV_GOTO_POS;                 
            }
        }
    }

/* #endregion */

/***********************************************************************************************************************************************/

/* #region DRIVE_ACTION_COMMAND */
    
    // Update current action and last action
    void Map2D::newDriveAction(uint8_t a) {
        driveAction_last = driveAction;
        driveAction = a;
    }

    // Determine the next drive command
    // Control sequence of actions required to achieve macro objective
    void Map2D::nextDriveCmd() {
        // Get current buffer command
        uint8_t currentBufferCommand = objectiveBuff[objectiveBuffIndex];
        // Check if next command in objective buffer is drive related (solo or tandem)
        if ((currentBufferCommand > 39 && currentBufferCommand < 60) || (currentBufferCommand > 79 && currentBufferCommand < 100)) { 
            newDriveAction(currentBufferCommand); 
        } else {
            newDriveAction(ACTION_WAIT);
        }
    }

/* #endregion */

/***********************************************************************************************************************************************/

/* #region ULTRASONIC_ACTION_COMMAND */

    // Update current action and last action
    void Map2D::newUSAction(uint8_t a) {
        usAction_last = usAction;
        usAction = a;
    }

    // Called to load in the next action from the buffer
    void Map2D::nextUSCmd() {
        // Get current buffer command
        uint8_t currentBufferCommand = objectiveBuff[objectiveBuffIndex];
        // Check if next command in objective buffer is US related (solo or tandem)
        if ((currentBufferCommand > 19 && currentBufferCommand < 40) || (currentBufferCommand > 79 && currentBufferCommand < 100)) { 
            newUSAction(currentBufferCommand); 
        } else {
            newUSAction(ACTION_WAIT);
        }
    }

    // Handle sweep action ultrasonic commands
    // Get 360 degrees of ping data at whatever resolution.
    void Map2D::actionSweep() {
        if (pingBuff_size == 0) {
            // Set servo operating mode to ping each time it reaches the command position
            // Set starting sweep position
            usCmd.function = CMD_SERVO_GOTO_PING;
            usCmd.pos = -90;
        } else if (pingBuff_size < 19) {
            // Increment servo position through entire sweep          
            usCmd.pos = usCmd.pos + 10;
        } else {
            usCmd.function = CMD_SERVO_GOTO_POS;
            usCmd.pos = 0;
            pingBuffToGrid(GRID_SOLID);
            //gridSolidify();            
            //gridMarkEmpty();
            newUSAction(ACTION_COMPLETE);
        }
    }

/* #endregion */

/***********************************************************************************************************************************************/

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
        // Mark buffer as empty
        pingBuff_size = 0;
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

/***********************************************************************************************************************************************/

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