#define DEBUG_TEST
#define DEBUG_IO_SIMULATE
#ifdef DEBUG_IO_SIMULATE
#include "IOSimulate.h"
IOSimulate ioSim;
Debounce testClock;
#endif

#include <Arduino.h>
#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "WiFiWebServer.h"
#include "UltrasonicRange.h"
#include "Compass.h"
#include "Map2D.h"
#include "Debounce.h"
#include "AwfulPID.h"

#define PIN_ENCODER 1
#define PIN_BUMPSENSOR 2

ArduinoLEDMatrix matrix;
WiFiWebServer server(80);
char ssid[] = "@";
char pass[] = "@";
UltrasonicRange us(1,2,400);
Compass compass;
Debounce encoderPulse;
Map2D floorMap;
AwfulPID pid_drive;
const PIDConfiguration pidCfg_drive {1, 0, 100, false};
const PIDParameters pidParam_drive {0.2, 0.5, 0.0};
// Scratch *********########
int stepperPos;

void initIO() {
  pinMode(PIN_ENCODER, INPUT);
  pinMode(PIN_BUMPSENSOR, INPUT);
}

void initMatrix() {
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
  matrix.begin();
  matrix.renderBitmap(frame, 8, 12);  
}

void initDevice() {
  compass.setup(); 
  encoderPulse.setup(digitalRead(PIN_ENCODER));
  server.setup(ssid, pass, 192, 168, 51, 236);  
}

void initAbstract() {
  floorMap.setup();
  for (int i=0; i<2499; i++) { floorMap.data[i] = 2; }  
  pid_drive.setConfig(pidCfg_drive);
  pid_drive.setParam(pidParam_drive);
}

void setup() {
  initMatrix();
  Serial.begin(9600);
  #ifdef DEBUG_TEST
    ioSim.setup();
    ioSim.setPinMode(1, 1);
    ioSim.setPinClock(1, 2000, 2000);
    ioSim.setPinMode(2,2);
    pid_drive.setConfig(pidCfg_drive);
    pid_drive.setParam(pidParam_drive);    
    return;
  #endif
  initIO();
  initDevice();
  initAbstract();
}

void loop() {
  #ifdef DEBUG_TEST
    int newSPCount;
    testClock.setup(0);
    int pv = 0;
    int sp = 100;
    int cv = 0;
    while(true) {
      ioSim.update();
      int clock;
      clock = (int) testClock.update(!testClock.getState(), 200, 200);
    }
  #endif

  // Answer client requests
  if (server.requestAvailable() != "") { server.respond(floorMap.data); }

  // Get commands from floorMap
  DriveCommand dc = floorMap.getDriveCommand();
  USCommand uc = floorMap.getUSCommand();

  // Get heading from compass
  int heading = compass.getHeading();

  // Control Motors
  if (dc.speed) {
    // Control loop for error between compass heading and command heading
    int speedBias = pid_drive.update(0x01, heading, dc.v.dir);
    // Use speedBias to increase one motor speed while decreasing the other, causing rotation
    int mtrSpeed_R = dc.speed + speedBias;
    int mtrSpeed_L = dc.speed - speedBias;
  }

  // Control Ultrasonic
  Vector vPing;
  if (stepperPos == uc.pos) {
    vPing.dir = heading + stepperPos;
    vPing.dist = us.getRangeCM();
  } else { vPing.dist = 0; }

  // Send events to floorMap
  if (encoderPulse.update(digitalRead(PIN_ENCODER), 50, 50)) { floorMap.step(); }
  if (vPing.dist) { floorMap.ping(vPing); }
  if (digitalRead(PIN_BUMPSENSOR)) { floorMap.collision(); }
  floorMap.update();

}


/*
Single very low resolution encoder attached to only one motor.
Can do just one motor because the turns should result in little to no displacement
Only displace on straight paths, only need to calculate on straight paths
Low resolution so an interrupt is not required to catch the pulse

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