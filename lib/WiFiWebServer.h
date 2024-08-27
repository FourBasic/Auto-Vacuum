#ifndef WIFIWEBSERVER_H
#define WIFIWEBSERVER_H

class WiFiWebServer {
  public:
		
		WiFiWebServer(int port);
		//void printWifiStatus();
		void setup(const char *ssid, const char *pass, uint8_t ipo1, uint8_t ipo2, uint8_t ipo3, uint8_t ipo4);
		void printWiFiStatus();
		int requestAvailable();
		
  private:
		WiFiServer server;
		int status = WL_IDLE_STATUS;
		String currentLine;
		
		
};
#endif
