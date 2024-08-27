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

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
int number = 0;

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void setup() {
  matrix.begin();
  matrix.renderBitmap(frame, 8, 12);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(5000);
  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // set the LED pin mode
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  WiFi.config(IPAddress(192,168,51,236));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(6000);

  for (int x=0; x<50; x++) {
    for (int y=0; y<50; y++) {
      if (y == 7) {
        floorMap[x][y] = 1;
      } else if (x == 35) {
        floorMap[x][y] = 2;
      }    
    }
  }
  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}

void loop() {
// compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      delayMicroseconds(10);                // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out to the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(F("<canvas id='myCanvas' width='1500' height='1500'></canvas>"));
            
            client.print(F("<script>var canvas=document.getElementById('myCanvas');var ctx=canvas.getContext('2d');var eSquare=20;var fSquare=20;var gridSize=50;var coordStrY='"));
            for (int x=0; x<50; x++) {
              for (int y=0; y<50; y++) {
                client.print(floorMap[x][y]);
              }
            }
            client.print(F("';var coordInd=0;"));
            client.print(F("ctx.strokeStyle='rgb(0,0,0)';for(let x=1; x<=gridSize;x+=1){for(let y=1;y<=gridSize;y+=1){ctx.strokeRect(x*eSquare,y*eSquare,eSquare,eSquare);"));
            client.print(F("if(coordInd<gridSize^2){if(coordStrY.charAt(coordInd)=='1')"));            
            client.print(F("{ctx.fillStyle='rgb(255,0,0)';ctx.fillRect(x*eSquare,y*eSquare,fSquare,fSquare);}"));
            client.print(F("else if(coordStrY.charAt(coordInd)=='2'){ctx.fillStyle='rgb(0,255,0)';"));
            client.print(F("ctx.fillRect(x*eSquare,y*eSquare,fSquare,fSquare);"));
            client.print(F("}}coordInd+=1;}}"));
            client.print(F("</script>"));
            client.print(F("<meta http-equiv=\"refresh\" content=\"10\">"));
            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(led, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

