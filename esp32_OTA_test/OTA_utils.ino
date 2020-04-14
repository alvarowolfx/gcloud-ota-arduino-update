void checkNewFirmware(){
	if (millis()-lastCheckedFirmwaresTime>checkNewFirmwareInterval) {
		// Execute OTA Update
		getBinName();
		if (lastFirmwareUploaded!=latestBin) {
			//only perform the ota update if the latest bin name found on the server is different from the one we already flashed (the bin name is stored in the eeprom)
			Serial.println("Starting firmware update procedure");
			execOTA();
		}else{
			Serial.println("I'm already runnign the latest firmware!");
		}
		lastCheckedFirmwaresTime=millis();
	}
}

void connectToWifi(){
	Serial.println("Connecting to " + String(SSID));

	// Connect to provided SSID and PSWD
	WiFi.begin(SSID, PSWD);

	// Wait for connection to establish
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print(".");   // Keep the serial monitor lit!
		delay(500);
	}

	// Connection Succeed
	Serial.println("");
	Serial.println("Connected to " + String(SSID));
}

String getDateString() {
	time_t rawtime = timeClient.getEpochTime();
	struct tm * ti;
	ti = localtime (&rawtime);

	uint16_t year = ti->tm_year + 1900;
	String yearStr = String(year);

	uint8_t month = ti->tm_mon + 1;
	String monthStr = month < 10 ? "0" + String(month) : String(month);

	uint8_t day = ti->tm_mday;
	String dayStr = day < 10 ? "0" + String(day) : String(day);

	return yearStr + "-" + monthStr + "-" + dayStr;
}

static String bucketName="remote-esp32-upload-firmwares";
static String apiEndPoint="https://storage.googleapis.com/storage/v1/b/"+bucketName+"/o";
const char* payload;

void getBinName(){
	String namePattern="firmware_esp32_"+getDateString();
	//String namePattern="firmware_esp32_2020-03-31";
	String urlParams= apiEndPoint+"?startOffset="+namePattern;

	Serial.println(urlParams);
	http.begin(urlParams);

	int httpCode = http.GET();//Make the request
	if (httpCode > 0) {     //Check for the returning code

		String response=http.getString();
		payload = response.c_str();

		// Parse JSON object
		DeserializationError error = deserializeJson(doc, payload);
		if (error) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			return;
		}else{
			int arraylength=doc["items"].size();
			String name= doc["items"][arraylength-1]["name"];
			latestBin=name;
		}
	}else {
		Serial.println("Error on HTTP request");
	}
	http.end(); //Free the resources
}


// OTA Logic
void execOTA() {


	Serial.println("Connecting to: " + String(host));
	// Connect to S3
	if (client.connect(host.c_str(), port)) {
		// Connection Succeed.
		// Fecthing the bin
		Serial.println("Fetching Bin: " + String(latestBin));

		// Get the contents of the bin file
		client.print(String("GET ") + binFolder+latestBin + " HTTP/1.1\r\n" +
		             "Host: " + host + "\r\n" +
		             "Cache-Control: no-cache\r\n" +
		             "Connection: close\r\n\r\n");

		// Check what is being sent
		//    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
		//                 "Host: " + host + "\r\n" +
		//                 "Cache-Control: no-cache\r\n" +
		//                 "Connection: close\r\n\r\n");

		unsigned long timeout = millis();
		while (client.available() == 0) {
			if (millis() - timeout > 5000) {
				Serial.println("Client Timeout !");
				client.stop();
				return;
			}
		}
		// Once the response is available,
		// check stuff

		/*
		   Response Structure
		    HTTP/1.1 200 OK
		    x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
		    x-amz-request-id: 2D56B47560B764EC
		    Date: Wed, 14 Jun 2017 03:33:59 GMT
		    Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
		    ETag: "d2afebbaaebc38cd669ce36727152af9"
		    Accept-Ranges: bytes
		    Content-Type: application/octet-stream
		    Content-Length: 357280
		    Server: AmazonS3

		    {{BIN FILE CONTENTS}}
		 */
		while (client.available()) {
			// read line till /n
			String line = client.readStringUntil('\n');
			// remove space, to check if the line is end of headers
			line.trim();

			// if the the line is empty,
			// this is end of headers
			// break the while and feed the
			// remaining `client` to the
			// Update.writeStream();
			if (!line.length()) {
				//headers ended
				break;     // and get the OTA started
			}

			// Check if the HTTP Response is 200
			// else break and Exit Update
			if (line.startsWith("HTTP/1.1")) {
				if (line.indexOf("200") < 0) {
					Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
					break;
				}
			}

			// extract headers here
			// Start with content length
			if (line.startsWith("Content-Length: ")) {
				contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
				Serial.println("Got " + String(contentLength) + " bytes from server");
			}

			// Next, the content type
			if (line.startsWith("Content-Type: ")) {
				String contentType = getHeaderValue(line, "Content-Type: ");
				Serial.println("Got " + contentType + " payload.");
				if (contentType == "application/octet-stream") {
					isValidContentType = true;
				}
			}
		}
	} else {
		// Connect to S3 failed
		// May be try?
		// Probably a choppy network?
		Serial.println("Connection to " + String(host) + " failed. Please check your setup");
		// retry??
		// execOTA();
	}

	// Check what is the contentLength and if content type is `application/octet-stream`
	Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

	lastFirmwareUploaded=latestBin;
	EEPROM.put( eeAddress, lastFirmwareUploaded );

	// check contentLength and content type
	if (contentLength && isValidContentType) {
		// Check if there is enough to OTA Update
		bool canBegin = Update.begin(contentLength);

		// If yes, begin
		if (canBegin) {
			Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
			// No activity would appear on the Serial monitor
			// So be patient. This may take 2 - 5mins to complete
			size_t written = Update.writeStream(client);

			if (written == contentLength) {
				Serial.println("Written : " + String(written) + " successfully");
			} else {
				Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
				// retry??
				// execOTA();
			}

			if (Update.end()) {
				Serial.println("OTA done!");
				if (Update.isFinished()) {
					Serial.println("Update successfully completed. Rebooting.");
					ESP.restart();
				} else {
					Serial.println("Update not finished? Something went wrong!");
				}
			} else {
				Serial.println("Error Occurred. Error #: " + String(Update.getError()));
			}
		} else {
			// not enough space to begin OTA
			// Understand the partitions and
			// space availability
			Serial.println("Not enough space to begin OTA");
			client.flush();
		}
	} else {
		Serial.println("There was no content in the response");
		client.flush();
	}
}
