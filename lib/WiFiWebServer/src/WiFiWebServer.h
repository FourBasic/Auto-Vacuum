#ifndef WIFIWEBSERVER_H
#define WIFIWEBSERVER_H
#include <Arduino.h>
#include "WiFiS3.h"

class WiFiWebServer {
  public:		
		WiFiWebServer(int port);
		void setup(const char *ssid, const char *pass, uint8_t ipo1, uint8_t ipo2, uint8_t ipo3, uint8_t ipo4);
		void printWiFiStatus();
		String requestAvailable();
		void respond(uint8_t data[2500]);		
  private:
		WiFiServer server;
		WiFiClient client;
		int status = WL_IDLE_STATUS;
		String request;		
};
#endif
