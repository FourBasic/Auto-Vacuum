#include "WiFiWebServer.h"
#include <Arduino.h>
#include "WiFiS3.h"

WiFiWebServer::WiFiWebServer(int port) {
    WiFiServer server(port);
}

void WiFiWebServer::printWiFiStatus() {
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

void WiFiWebServer::setup(const char *ssid, const char *pass, uint8_t ipo1, uint8_t ipo2, uint8_t ipo3, uint8_t ipo4) {
    if (!Serial) { Serial.begin(9600); }
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    delay(5000);
    Serial.println("Access Point Web Server");

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
    WiFi.config(IPAddress(ipo1,ipo2,ipo3,ipo4));

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
    delay(10000);

      // start the web server on port 80
    server.begin();

  // you're connected now, so print out the status
    printWiFiStatus();
}

String WiFiWebServer::requestAvailable() {
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

    client = server.available();   // listen for incoming clients
    if (client) {                            // if you get a client,
        Serial.println("new client");           // print a message out the serial port
        String currentLine = "";                       // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            delayMicroseconds(10);                // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out to the serial monitor
                if (c == '\n') {                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        return request;
                        break;
                    } else {      // if you got a newline, then clear currentLine:
                    request = currentLine;
                    currentLine = "";
                    }
                } else if (c != '\r') {    // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }        
            }
        }
    }
    return "";
}

void WiFiWebServer::respond(uint8_t data[2500]) {
    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
    // and a content-type so the client knows what's coming, then a blank line:
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    // the content of the HTTP response follows the header:
    client.print(F("<!DOCTYPE html>"));
    client.print(F("<canvas id='myCanvas' width='1500' height='1500'></canvas>"));
    
    client.print(F("<script>var canvas=document.getElementById('myCanvas');var ctx=canvas.getContext('2d');var eSquare=20;var fSquare=20;var gridSize=50;var coordStrY='"));
    for (int i=0; i<2500; i++) {
        client.print(data[i]);
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
            // close the connection:
    client.stop();
    Serial.println("client disconnected");
}