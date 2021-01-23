#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

/*
#include <Hash.h>
#include <ESPCrypto.h>
#include <base64.h>
*/

#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define LED_RED 3
#define LED_GREEN 2
#define LED_BLUE 0

#define WIFI_SSID "YOUR-SSID-HERE"
#define WIFI_PASS "YOUR-WIFI-PASSWORD"
#define OBS_WEBSOCKET_IP "192.168.X.X" //Add OBS IP
#define OBS_WEBSOCKET_PORT 4444
#define OBS_WEBSOCKET_PASS "CURRENTLY NOT WORKING"
#define OBS_SCENE_NAME "SCENE-SUBSTRING" //"Cam 1"

boolean authRequired = false;
boolean checkAuth = true;
boolean connected = false;
long messageIdCounter = 2;
boolean redLedStatus = 1;
boolean greenLedStatus = 1;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
      connected = false;
      redLedStatus = 1;
      greenLedStatus = 1;
      digitalWrite(LED_GREEN, 1);
      digitalWrite(LED_RED, 1);
      digitalWrite(LED_BLUE, 0);
      delay(100);
      digitalWrite(LED_BLUE, 1);
      delay(100);
      digitalWrite(LED_BLUE, 0);
      delay(100);
      digitalWrite(LED_BLUE, 1);
      delay(100);
      digitalWrite(LED_BLUE, 0);
      delay(100);
      digitalWrite(LED_BLUE, 1);
      delay(100);
      digitalWrite(LED_BLUE, 0);
			break;
      
		case WStype_CONNECTED:
      connected = true;
      digitalWrite(LED_GREEN, 1);
      digitalWrite(LED_RED, 1);
      digitalWrite(LED_BLUE, 1);
			break;
      
		case WStype_TEXT: {
      StaticJsonDocument<1024> resDoc;
       // Deserialize the JSON document
      DeserializationError error = deserializeJson(resDoc, payload);

      // Test if parsing succeeds.
      if (error) {
        return;
      }
      
      String messageId = resDoc["message-id"];
      if(messageId == "1"){
        checkAuth = false;
        authRequired = resDoc["authRequired"];

        /*
        USE_SERIAL.printf("DEBUG 1");
        SHA256 hasher;
        byte shaResult[SHA256_SIZE];
        
        USE_SERIAL.printf("DEBUG 2");
          
        String salt = resDoc["salt"];
        String challenge = resDoc["challenge"];
        
        USE_SERIAL.printf("DEBUG 3");
        
        String secretString = OBS_WEBSOCKET_PASS + salt;
        int strLength = secretString.length();
        char* secretCharArr;
        secretString.toCharArray(secretCharArr, strLength);

        USE_SERIAL.printf("DEBUG 4");
        
        hasher.doUpdate(secretCharArr);
        hasher.doFinal(shaResult);
        
        USE_SERIAL.printf("DEBUG 5");
        
        String result = (char*) shaResult;
        String secret = base64::encode(result + challenge);
        strLength = secret.length();
        char* secretArr;
        secret.toCharArray(secretArr, strLength);
        hasher.doUpdate(secretArr);
        hasher.doFinal(shaResult);
        result = (char*) shaResult;
        String authResponse = base64::encode(result);

        StaticJsonDocument<512> responseDoc;
        responseDoc["request-type"] = "Authenticate";
        responseDoc["message-id"] = messageIdCounter++;
        responseDoc["auth"] = authResponse;
        String responseBody;        
        webSocket.sendTXT(responseBody); */    
      }
      else{
        String updateType = resDoc["update-type"];
        if(updateType == "SwitchScenes"){
          String sceneName = resDoc["scene-name"];
          if(sceneName.indexOf(OBS_SCENE_NAME) >= 0){
            redLedStatus = 0;
            return;
          }
          redLedStatus = 1;
        }

        if(updateType == "PreviewSceneChanged"){
          String sceneName = resDoc["scene-name"];
          if(sceneName.indexOf(OBS_SCENE_NAME) >= 0){
            greenLedStatus = 0;
            return;
          }
          greenLedStatus = 1;
        }
      }
			break;
		}
      
    case WStype_ERROR:
      break;
      
    case WStype_PING:
        // pong will be send automatically
        break;
        
    case WStype_PONG:
        // answer to a ping we send
        break;
    }

}

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, 1);
  digitalWrite(LED_GREEN, 1);
  digitalWrite(LED_BLUE, 1);

  //Wait for boot
	delay(4000);

	WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

	//WiFi.disconnect();
	while(WiFiMulti.run() != WL_CONNECTED) {
		digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 1);
    digitalWrite(LED_BLUE, 0);
    Serial.print(".");
    delay(250);
    digitalWrite(LED_BLUE, 1);
    Serial.print(".");
    delay(250);
	}
  digitalWrite(LED_BLUE, 1);

	// server address, port and URL
	webSocket.begin(OBS_WEBSOCKET_IP, OBS_WEBSOCKET_PORT, "/");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// try every 5000ms again if connection has failed
	webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocket.enableHeartbeat(15000, 3000, 2);

  delay(5000);
}

void loop() {
	webSocket.loop();

  if(connected){
    if(!authRequired){
      if(checkAuth){
        webSocket.sendTXT("{\"request-type\":\"GetAuthRequired\",\"message-id\":\"1\"}");
      }
      if(redLedStatus == 0){
        digitalWrite(LED_RED, redLedStatus);
        digitalWrite(LED_GREEN, 1);
      }
      else{
        digitalWrite(LED_RED, redLedStatus);
        digitalWrite(LED_GREEN, greenLedStatus);
      }
    }
    else{
      for(int i=0; i<15; i++){
        digitalWrite(LED_GREEN, 1);
        digitalWrite(LED_RED, 0);
        delay(500);
        digitalWrite(LED_GREEN, 0);
        digitalWrite(LED_RED, 1);
        delay(500);
      }
      authRequired = false;
      checkAuth = true;
    }
  }
}
