#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_AHTX0.h>
#include <ArduinoJson.h>


// Replace with your network credentials
const char* ssid = "Sharif-WiFi";
//const char* password = "mamad1382";
long long int counter = 0;
// Server URL
const char* serverUrl = "http://172.27.10.192:5000/";
StaticJsonDocument<200> doc;
boolean changable7 =true;
boolean changable8 =true;
#define Pin008 16
#define Pin007 32

#define LED1  19
#define LED2  27
String pin7 ="0/0/7";
String pin8 ="0/0/8";
int LED1_state= HIGH;
int LED2_state = HIGH;
int state_7 = 0;  // input state key 1
int state_8 = 0;  // input state key 2
int Last_7 = 0;   // last input state key 1
int Last_8 = 0;   // last input state key 2
boolean condChange7 = false;
boolean condChange8 = false;
int on7=0;
int on8=0;


void setup() {
  pinMode(Pin007, INPUT);
  pinMode(Pin008, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(115200);
  // Connect to Wi-Fi
  WiFi.begin(ssid);
  // Create JSON document
  doc["addressId"] = "008";
  doc["value"] = "1";
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {

  state_7 = digitalRead(Pin007);
  state_8 = digitalRead(Pin008);


  if(state_7==LOW&&changable7){
    condChange7=true;
    doc["addressId"]=pin7;
    on7 = !(on7);
    doc["value"] = on7;
    LED1_state=!(LED1_state);
  }

  if(state_8==LOW&&changable8){
    condChange8=true;
    doc["addressId"]=pin8;
    on8 = !(on8);
    doc["value"] = on8;
    LED2_state=!(LED2_state);
  }


  

  if (WiFi.status() == WL_CONNECTED) {
    counter++;
    // Send HTTP GET request
    HTTPClient http;

    // Send HTTP POST request
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    

    String jsonString;
    serializeJson(doc, jsonString);

    

    if ((condChange8||condChange7)) {
      int httpResponseCode = http.POST(jsonString);
      String response = http.getString();
      Serial.println("HTTP POST Response code: " + String(httpResponseCode));
      Serial.println("POST Response: " + response);
      if(condChange7){
      condChange7=false;
      }
      if(condChange8){
        condChange8=false;
      }
    }
    
    http.end(); // Free resources
  }
  
  
  
  // no connection
   else {
    Serial.println("WiFi Disconnected");
  }
  if(state_7==LOW){
  changable7 = false;
  }
  else{
  changable7=true;
  }
  if(state_8==LOW){
  changable8 = false;
  }
  else{
  changable8=true;
  }
  digitalWrite(LED1, LED1_state);
  digitalWrite(LED2, LED2_state);
  delay(10);
}
