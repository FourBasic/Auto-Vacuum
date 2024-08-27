#include <Arduino.h>
#include "Arduino_LED_Matrix.h"
#include "Debounce.h"
#include "WiFiS3.h"

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
WiFiServer server(80);
int floorMap[50][50];
char ssid[] = "WunnDebb";        // your network SSID (name)
char pass[] = "pGhJC62m";        // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;

void setup() {
  matrix.begin();
  matrix.renderBitmap(frame, 8, 12);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  for (int x=0; x<50; x++) {
    for (int y=0; y<50; y++) {
      if (y == 7) {
        floorMap[x][y] = 1;
      } else if (x == 35) {
        floorMap[x][y] = 2;
      }    
    }
  }
}

void loop() {

}

