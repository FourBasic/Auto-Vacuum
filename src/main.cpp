/* #region PROG_CONFIG */
//#define DEBUG_TEST
//#define DEBUG_IO_SIMULATE
#ifdef DEBUG_IO_SIMULATE
#include "IOSimulate.h"
IOSimulate ioSim;
Debounce testClock;
#endif
/* #endregion */

/* #region INCLUDE */
#include <Arduino.h>
#include "Servo.h"
//#include "Arduino_LED_Matrix.h"
#include "GeneralFunctions.h"
#include "WiFiS3.h"
#include "WiFiWebServer.h"
#include "UltrasonicRange.h"
#include "Compass.h"
#include "Map2D.h"
#include "Debounce.h"
#include "AwfulPID.h"
#include "psw.h"
#include "ArduinoOTA.h"
/* #endregion */

/* #region DEFINE_IO_PINS */
#define PIN_ENCODER A3
#define PIN_ENCODER_ALT A2
#define PIN_US_ECHO A1
#define PIN_BUMPSENSOR A0
#define PIN_MTRL_REV 2
#define PIN_SERVO 3
#define PIN_MTRL_FWD 4
#define PIN_MTRL_SPD 5
#define PIN_MTRR_SPD 6
#define PIN_MTRR_FWD 7
#define PIN_MTRR_REV 8
#define PIN_US_TRIG 12
/* #endregion */

/* #region CONFIG*/
const PIDConfiguration pidCfg_drive {1, -70, 70, false, 10, 50};
const PIDParameters pidParam_drive {9.0, 0.0, 0.0};
/* #endregion */

/* #region GLOBAL_VAR */
//ArduinoLEDMatrix matrix;
WiFiWebServer server(80);
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PSWD;
UltrasonicRange us(PIN_US_TRIG,PIN_US_ECHO,400);
Servo myservo;
Compass compass;
Debounce encoderPulse;
Map2D floorMap;
AwfulPID pid_drive;
// Scratch *********########
int servoPos = 0;
unsigned long scanT_millis, scanT_millisLast;
unsigned int scanT_last, scanT_high;
unsigned int scanT_low = 32767;
float scanT_ave;

/* #endregion */

/* #region SETUP */
void initIO() {
  pinMode(PIN_US_ECHO, INPUT);
  pinMode(PIN_ENCODER, INPUT);
  pinMode(PIN_BUMPSENSOR, INPUT);
  pinMode(PIN_ENCODER_ALT, INPUT);

  pinMode(PIN_US_TRIG, OUTPUT);
  pinMode(PIN_MTRL_FWD, OUTPUT);
  pinMode(PIN_MTRL_REV, OUTPUT);
  pinMode(PIN_MTRL_SPD, OUTPUT);
  pinMode(PIN_MTRR_FWD, OUTPUT);
  pinMode(PIN_MTRR_REV, OUTPUT);
  pinMode(PIN_MTRR_SPD, OUTPUT);
  pinMode(PIN_SERVO, OUTPUT);
}

void initMatrix() {
  /*
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
  */
}

void initDevice() {
  compass.setup();
  encoderPulse.setup(digitalRead(PIN_ENCODER));
  server.setup(ssid, pass, 192, 168, 51, 236);
  myservo.attach(PIN_SERVO, 760,2260); //760,2260
}

void initAbstract() {
  floorMap.setup();
  //for (int i=0; i<2499; i++) { floorMap.data[i] = 2; }  
  pid_drive.setConfig(pidCfg_drive);
  pid_drive.setParam(pidParam_drive);
}

void initTest() {
  //ioSim.setup();
  //ioSim.setPinMode(1, 1);
  //ioSim.setPinClock(1, 2000, 2000);
  //ioSim.setPinMode(2,2);
  pid_drive.setConfig(pidCfg_drive);
  pid_drive.setParam(pidParam_drive);
  delay(1000);
  Serial.println("Hello");
  delay(1000);
  if (compass.setup() == -1) { while(1) {} }
  delay(200);
  Serial.println("Here We Go");
}

void setup() {
  //initMatrix();
  Serial.begin(9600);
  initIO();
  initDevice();
  initAbstract();
}
/* #endregion */

/* #region FUNCTION */
void controlMotor() {
  DriveCommand dc = floorMap.getDriveCommand();
  int heading = compass.get180();
  if (dc.speed) {
    // Control loop for error between compass heading and command heading
    // ****** Need to manipulate SP because of 360deg rollover and optimal path CW/CCW
    int pv_driveDir = heading;
    int sp_driveDir = dc.v.dir;
    //int diffCW = 
    //int diffCCW =  
    
    int speedBias = pid_drive.update(PID_ENABLE, heading, dc.v.dir);
    
    // If error is large (unstable), default to turning on the spot. Otherwise take speed cmd.
    int forwardSpeed = 0;
    if (pid_drive.getStability()) { forwardSpeed = dc.speed; }
    // Use speedBias to increase one motor speed while decreasing the other, causing rotation
    int mtrSpeed_R = dc.speed + speedBias;
    int mtrSpeed_L = dc.speed - speedBias;
  }
}

void controlUltrasonic() {
  USCommand uc = floorMap.getUSCommand();

  // Move servo to position
  if (uc.function != CMD_SERVO_HOLD) {
    int xpos = map(uc.pos,-180,180,5,179);
    myservo.write(xpos); //98 //8
    delay(300);
    servoPos = uc.pos;
  }

  //  Ping when servo is in position
  //Serial.println(servoPos);
  if (uc.function == CMD_SERVO_GOTO_PING && servoPos == uc.pos) {
    Vector vPing;
    //vPing.dir = heading360 + servoPos;
    vPing.dir = servoPos;
    vPing.dist = us.getRangeCM();
    floorMap.ping(vPing);  
  }
}

String dataToJSON() {
  String text = "{\"watch\":";
  text += String(scanT_ave, 0);
  text += ",\"watchH\":";
  text += String(scanT_high);
  text += ",\"watchL\":";
  text += String(scanT_low);
  text += "}";
  return text;
}

void watchdog() {
  scanT_millis = millis();  
  if (scanT_millisLast != 0 && scanT_millis > scanT_millisLast) {
    scanT_last = (scanT_millis - scanT_millisLast) ;/// 1000;
    if (scanT_last > scanT_high) { 
      scanT_high = scanT_last; 
    } else if (scanT_last < scanT_low) { 
      scanT_low = scanT_last; 
    }
    // Moving average - pretend 3000 in the buffer
    if (scanT_ave > 0) { 
      scanT_ave = ((scanT_ave * 3000) + (float)scanT_last) / 3001;
    } else {
      scanT_ave = (float)scanT_last;
    }
  }
  scanT_millisLast = scanT_millis;

}
/* #endregion */

void loop() {
  if (server.requestAvailable()) { server.respond(dataToJSON()); }
  
  compass.update();
  controlMotor();
  controlUltrasonic();  

  if (encoderPulse.update(digitalRead(PIN_ENCODER), 50, 50)) { floorMap.step(); }
  if (digitalRead(PIN_BUMPSENSOR)) { floorMap.collision(); }

  floorMap.update();
  watchdog();
}

/* #region NOTES */
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
/* #endregion */