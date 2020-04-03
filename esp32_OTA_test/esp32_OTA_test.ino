#include "Arduino.h"

#include <WiFi.h>
#include <Update.h>
#include <HTTPClient.h>

#include <tinyxml2.h>
using namespace tinyxml2;

WiFiClient client;
HTTPClient http;

// Variables to validate
// response from S3
long contentLength = 0;
bool isValidContentType = false;

// Your SSID and PSWD that the chip needs
// to connect to
const char* SSID = "romagnetti";
const char* PSWD = "robocats";


//http://storage.googleapis.com/remote-esp32-upload-firmwares/firmware_esp32.bin
// S3 Bucket Config
String host = "storage.googleapis.com"; // Host => bucket-name.s3.region.amazonaws.com
int port = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String binFolder = "/remote-esp32-upload-firmwares/"; // bin file name with a slash in front.
const char* latestBin;

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
	return header.substring(strlen(headerName.c_str()));
}

void setup() {
	//Begin Serial
	Serial.begin(115200);
	delay(10);

	connectToWifi();

	// Execute OTA Update
	getBinName();
	execOTA();
}

void loop() {
	// chill
}
