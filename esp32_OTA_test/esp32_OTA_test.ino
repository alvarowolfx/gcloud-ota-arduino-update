#include "Arduino.h"

#include <WiFi.h>
WiFiClient client;

#include <Update.h>
#include <HTTPClient.h>
HTTPClient http;

#include <WiFiUdp.h>
WiFiUDP ntpUDP;

// #include <tinyxml2.h>
// using namespace tinyxml2;
#include <ArduinoJson.h>


#include <NTPClient.h>
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

#include <EEPROM.h>

// Variables to validate
// response from S3
long contentLength = 0;
bool isValidContentType = false;

// Your SSID and PSWD that the chip needs
// to connect to
const char* SSID = "romagnetti";
const char* PSWD = "robocats";


int checkNewFirmwareInterval=1000*30; //30 seconds
long lastCheckedFirmwaresTime=0;

StaticJsonDocument<10000> doc;

//http://storage.googleapis.com/remote-esp32-upload-firmwares/firmware_esp32.bin
// S3 Bucket Config
String host = "storage.googleapis.com"; // Host => bucket-name.s3.region.amazonaws.com
int port = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String binFolder = "/remote-esp32-upload-firmwares/"; // bin file name with a slash in front.
String latestBin;
char lastFirmwareUploaded[40];
int eeAddress = 0; //EEPROM address to start reading from

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
	return header.substring(strlen(headerName.c_str()));
}

void setup() {
	//Begin Serial
	Serial.begin(115200);
	delay(10);

	connectToWifi();
	timeClient.begin();

	if (!EEPROM.begin(1000)) {
		Serial.println("Failed to initialise EEPROM");
		Serial.println("Restarting...");
		delay(1000);
		ESP.restart();
	}

	EEPROM.get( eeAddress, lastFirmwareUploaded );
	Serial.print("I'm running firmware: ");
	Serial.println(lastFirmwareUploaded);
}

void loop() {
	checkNewFirmware();
	timeClient.update();

	delay(5000);
}
