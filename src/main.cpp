#include <Arduino.h>
#include "Arduino_LED_Matrix.h"
#include "WiFiS3.h"
#include "WiFiWebServer.h"
#include "UltrasonicRange.h"

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

UltrasonicRange us(1,2,400);

uint8_t floorMap[2500];

void setup() {
  matrix.begin();
  matrix.renderBitmap(frame, 8, 12);
  Serial.begin(9600);
  for (int i=0; i<2499; i++) {
        floorMap[i] = 2;
  }
  server.setup(ssid, pass, 192, 168, 51, 236);
}

void loop() {  
  if (server.requestAvailable() != "") { server.respond(floorMap); }
}