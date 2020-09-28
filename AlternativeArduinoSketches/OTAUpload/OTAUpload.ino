#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

/* fill your ssid and password here */
const char* ssid = "romagnetti";
const char* password = "robocats";

long lastMsg = 0;
int flag = false;

/* LED is pin GIO12 */
int led = 12;

void setup() {
  
  Serial.begin(115200);
  /* connect to wifi */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  /* Wait for connection */
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  /* set LED as output */
  pinMode(led, OUTPUT);
  
  /* create a connection at port 3232 */
  ArduinoOTA.setPort(3232);
  /* we use mDNS instead of IP of ESP32 directly */
  ArduinoOTA.setHostname("esp32");

  /* we set password for updating */
  ArduinoOTA.setPassword("iotsharing");

  /* this callback function will be invoked when updating start */
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating");
  });
  /* this callback function will be invoked when updating end */
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd updating");
  });
  /* this callback function will be invoked when a number of chunks of software was flashed
  so we can use it to calculate the progress of flashing */
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  /* this callback function will be invoked when updating error */
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  /* start updating */
  ArduinoOTA.begin();
  Serial.print("ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /* this function will handle incomming chunk of SW, flash and respond sender */
  ArduinoOTA.handle();

  /* we can not use delay() here, because it will block the ArduinoOTA work
  so we count the time, if it reach we toggle the lED */
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    if(flag == false){
      digitalWrite(led, HIGH); 
      flag = true;
    }else{
      digitalWrite(led, LOW); 
      flag = false;      
    }
  }
}
