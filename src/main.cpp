#include <Arduino.h>
#include "Arduino_LED_Matrix.h"
#include "Debounce.h"
#include "WiFiS3.h"
#include "WiFiWebServer.h"

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
uint8_t floorMap[2500];
WiFiWebServer server(80);
char ssid[] = "WunnDebb";        // your network SSID (name)
char pass[] = "pGhJC62m";        // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

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
  if (server.requestAvailable() != "") { server.respond(floorMap); Serial.println("gurpy bad"); };
}