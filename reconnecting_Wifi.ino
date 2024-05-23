#include <WiFi.h>
 
const char* ssid = "Pioneer ";
const char* password = "mamad1382";
const int ledPinOn =  27;
const int ledPinRec =  26;
int ledState27 = LOW;
int ledState26 = LOW;


void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Connected to AP successfully!");
  ledState27= HIGH;
  digitalWrite(ledPinOn, ledState27);
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  ledState27= LOW;
  if (ledState26 == LOW) {
      ledState26 = HIGH;
  } else {
    ledState26 = LOW;
  }
  digitalWrite(ledPinRec, ledState26);
  digitalWrite(ledPinOn, ledState27);
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}

void setup(){
  Serial.begin(115200);
  pinMode(ledPinOn, OUTPUT);
  pinMode(ledPinRec, OUTPUT);
  // delete old config
  WiFi.disconnect(true);

  delay(1000);

  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  /* Remove WiFi event
  Serial.print("WiFi Event ID: ");
  Serial.println(eventID);
  WiFi.removeEvent(eventID);*/

  WiFi.begin(ssid, password);
    
  Serial.println();
  Serial.println();
  Serial.println("Wait for WiFi... ");
}

void loop(){
  delay(500);
}
