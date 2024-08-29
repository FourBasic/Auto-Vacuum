#include <Arduino.h>
#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "WiFiWebServer.h"
#include "UltrasonicRange.h"
#include "Compass.h"
#include "Map2D.h"

ArduinoLEDMatrix matrix;
byte frame[8][12] = {
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },

  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 },

  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },

  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },

  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },

  { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },

  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },

  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

WiFiWebServer server(80);
char ssid[] = "WunnDebb";
char pass[] = "pGhJC62m";

UltrasonicRange usf(1,2,400);
UltrasonicRange usr(3,4,400);
Compass compass;

Map2D floorMap;

void setup() {
  matrix.begin();
  matrix.renderBitmap(frame,8,12);
  Serial.begin(9600);
  for (int i=0; i<2499; i++) { floorMap.setDataPoint(i,2); }
  server.setup(ssid,pass,192,168,51,236);
  compass.setup();
  floorMap.setup();
}

void loop() {  
  if (server.requestAvailable() != "") { server.respond(floorMap.getDataBlock()); }
  Vector vf, vr;
  vf.dist = usf.getRangeCM();
  vr.dist = usr.getRangeCM();
  compass.update();
  vf.dir = compass.getHeading180();
  vr.dir = vf.dir;

  floorMap.updatePosition(vf, vr);

}


/*
compass on frame.
stepper can be deduced from step pos relative to frame.
stepper is homed using US to ping a blocking flag on the frame.
encoder to compress transmitted data to browser --> 0 0 1 1 1 1 2 2 2 2 3 2 2 --> 0=10,1=11,2=12 followed by conseq qty --> 10 2 11 4 12 4 3 12 2 ---> 9 txByte vs 13 txByte 
multiple us?

1-  rotate stepper / ping until close range marker ref.
    now body and stepper can be synced so the stepper always adjusts to be pointing in a compass direction (as a test)

2-  if teach mode then the goal is to find the rough outline of the entire space. 
    The rough outline will allow preplanning actual destinations so it doesn't wander around aimlessly.
      stop -> 360 scan -> mark unknowns (0) with empties (1) or full (2)
      path to next scan location -> stop --> scan --> populate new unknowns and reconcile descrepancies with previous scans.
      path finding can just be drawing a straight line in any direction, if it hits the "border" instead of a boundary line
      then go there.
    
3-  if run mode the goal should be running certain lawnmower style function in small chunks of the "map" until entire map is complete



*/