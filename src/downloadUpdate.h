#include <Arduino.h>

#include <DNSServer.h>
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266Wifi.h>
#include <ESP8266HTTPClient.h>
#else
#include <WiFi.h>
#include <Update.h>
#endif

bool downloadUpdate(String filename){  
  int contentLength = 0;
  bool isValidContentType = false;

  USE_SERIAL.println("Connecting to: " + String(bucket) + String(filename));  
  if (client.connect(bucket.c_str(), 80)) {    
    USE_SERIAL.println("Fetching Bin: " + String(filename));
    // Get the contents of the bin file
    client.print(String("GET ") + filename + " HTTP/1.1\r\n" +
                 "Host: " + bucket + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        USE_SERIAL.println("Client Timeout !");
        client.stop();
        return false;
      }
    }
    
    while (client.available()) {      
      String line = client.readStringUntil('\n');      
      line.trim();
      
      if (!line.length()) {
        break; 
      }
      
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          USE_SERIAL.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }
      
      if (line.startsWith("Content-Length: ")) {
        contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
        USE_SERIAL.println("Got " + String(contentLength) + " bytes from server");
      }
      
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        USE_SERIAL.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {    
    USE_SERIAL.println("Connection to " + String(host) + " failed. Please check your setup");
    return false;    
  }
  
  USE_SERIAL.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  if (contentLength && isValidContentType) {
    bool canBegin = Update.begin(contentLength);    
    if (canBegin) {
      USE_SERIAL.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");      
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        USE_SERIAL.println("Written : " + String(written) + " successfully");
      } else {
        USE_SERIAL.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );        
      }

      if (Update.end()) {
        USE_SERIAL.println("OTA done!");
        if (Update.isFinished()) {
          USE_SERIAL.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          USE_SERIAL.println("Update not finished? Something went wrong!");
        }
      } else {
        USE_SERIAL.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {      
      USE_SERIAL.println("Not enough space to begin OTA");
      client.flush();
      return false;
    }
  } else {
    USE_SERIAL.println("There was no content in the response");
    client.flush();
    return false;
  }  
}