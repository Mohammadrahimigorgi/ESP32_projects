/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-wi-fi-manager-asyncwebserver/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "LittleFS.h"
#include <HTTPClient.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

void LEDhandles();
void serverConnect();
void IRAM_ATTR press_LED1();
void IRAM_ATTR press_LED2();
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
long long int counter = 0;
StaticJsonDocument<200> doc;

int counter_press_LED1 = 0;
int counter_press_LED2 = 0;

#define LED2_Input 16 // input key pin
#define LED1_Input 32 // input key pin


#define LED1  19  // LED1 pin output
#define LED2  27  // LED2 pin output


String press_pin1 ="0/0/1"; // single and ordinary press on button will send this data for LED1
String long_press_pin1 ="0/0/2";  //long press will send this data for LED1
String double_press_pin1 ="0/0/3";  //double press will send this data for LED1

String press_pin2 ="0/0/4"; // single and ordinary press on button will send this data for LED2
String long_press_pin2 ="0/0/5";  //long press will send this data for LED2
String double_press_pin2 ="0/0/6";  //double press will send this data for LED2

boolean post = false;
boolean restarted_LED7 = true;
boolean restarted_LED8 = true;

int mode = 0;

int LED1_state = HIGH; // LED1 on and off state 
int LED2_state = HIGH;  // LED2 on and off state 

int LED1_key_state ; 
int LED2_key_state ; 

boolean double_press_LED1_occur;
boolean double_press_LED2_occur;

long unsigned fall2_LED7=0;
long unsigned fall_LED7=0;

long unsigned fall2_LED8=0;
long unsigned fall_LED8=0;
// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";
boolean action = false;
IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)



// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());


  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}

// Replaces placeholder with LED state value

void setup() {
  pinMode(LED1_Input, INPUT);
  pinMode(LED2_Input, INPUT);
  // declaring interupt
  attachInterrupt(digitalPinToInterrupt(LED1_Input),press_LED1 , FALLING);
  attachInterrupt(digitalPinToInterrupt(LED2_Input),press_LED2 , FALLING);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);



  // Serial port for debugging purposes
  Serial.begin(115200);

  initLittleFS();
  
  // Load values saved in LittleFS
  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ip = "http://"+readFile(LittleFS, ipPath);
  gateway = readFile (LittleFS, gatewayPath);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);

  if(initWiFi()) {
  Serial.println("connected to wifi : " + ssid);
  Serial.println("IP server : " + ip);
  HTTPClient http;
  http.begin(ip);
  int httpResponseCode = http.GET();
  String response = http.getString();
  Serial.println(httpResponseCode);
  Serial.print("response of server is : ");
  Serial.println(response);  
  Serial.println("end http");
  ArduinoOTA.begin();
  Serial.print("IP Adrress : ");
  Serial.println(WiFi.localIP());
  action = true;

  }
  else {
    serverConnect();
    ArduinoOTA.begin();
    Serial.print("IP Adrress : ");
    Serial.println(WiFi.localIP());
}
}

void loop() {

ArduinoOTA.handle();
if(action){
LEDhandles();
}
}


void LEDhandles(){

  unsigned long currentMillis = millis(); //real time value
  LED1_key_state=digitalRead(LED1_Input);
  LED2_key_state=digitalRead(LED2_Input);

  if(LED1_key_state==HIGH && currentMillis > 500+fall_LED7 && counter_press_LED1 == 1 ){
    mode = 1;
    post = true;
    doc["addressId"] = "0/0/1";

    Serial.println("single press LED1 detected");
    counter_press_LED1 = 0;
  }

  if(LED1_key_state==LOW && currentMillis > 1500+fall_LED7 && counter_press_LED1 == 1 ){
    mode = 2;
    post = true;
    doc["addressId"] = "0/0/2";

    Serial.println("long press LED1 detected");
    counter_press_LED1 = 0;
  }


    if(LED2_key_state==HIGH && currentMillis > 500+fall_LED8 && counter_press_LED2 == 1 ){
    mode = 4;
    post = true;
    doc["addressId"] = "0/0/4";

    Serial.println("single press LED2 detected");
    counter_press_LED2 = 0;
  }

  if(LED2_key_state==LOW && currentMillis > 1500+fall_LED8 && counter_press_LED2 == 1 ){
    mode = 5;
    post = true;
    doc["addressId"] = "0/0/5";

    Serial.println("long press LED2 detected");
    counter_press_LED2 = 0;
  }
  if(fall2_LED7 - fall_LED7<500 && counter_press_LED1>0){
    double_press_LED1_occur=true;
  }
  if(fall2_LED8 - fall_LED8<500 && counter_press_LED2>0){
    double_press_LED2_occur=true;
  }

  if(double_press_LED1_occur){
    mode = 3;
    post = true;
    doc["addressId"] = "0/0/3";

    Serial.println("double press LED1 detected");
    counter_press_LED1 = 0;
  }

  if(double_press_LED2_occur){

    mode = 6;
    post = true;
    doc["addressId"] = "0/0/6";

    Serial.println("double press LED2 detected");
    counter_press_LED2 = 0;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
// condition for start post

    if (post && mode>0) {
      http.begin(ip);
      http.addHeader("Content-Type", "application/json");
      if(mode >3 ){
        doc["value"] = !LED2_state;
      }
      else{

        doc["value"] = !LED1_state;
      }
      
      String jsonString;
      serializeJson(doc, jsonString);
      int httpResponseCode = http.POST(jsonString);
      String response = http.getString();
      if(httpResponseCode>0 && post){
      Serial.println("HTTP POST Response code: " + String(httpResponseCode));
      Serial.println("POST Response: " + response);
      counter_press_LED1 = 0;
      counter_press_LED2 = 0;
      post = false;
      double_press_LED1_occur=false;
      double_press_LED2_occur=false;
      mode = 0;
      }
      else{
        Serial.println("no Response");
      }
    http.end(); // Free resources
    delay(1000);
  
    }
  }
    // no connection
   else {
    Serial.println("WiFi Disconnected");
  }



  digitalWrite(LED1, LED1_state);
  digitalWrite(LED2, LED2_state);
  delay(300);

}












void serverConnect(){

    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-Key", NULL);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html");
    });
    
    server.serveStatic("/", LittleFS, "/");
    
    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(LittleFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(LittleFS, passPath, pass.c_str());
          }
          // HTTP POST ip value
          if (p->name() == PARAM_INPUT_3) {
            ip = p->value().c_str();
            Serial.print("IP Address set to: ");
            Serial.println(ip);
            // Write file to save value
            writeFile(LittleFS, ipPath, ip.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
      delay(3000);
      ESP.restart();
    });
    server.begin();

}


void IRAM_ATTR press_LED1(){
  if(counter_press_LED1==0){
  fall_LED7= millis();
  Serial.println("first tap");
  }
  if(counter_press_LED1==1){
  fall2_LED7=millis();
  Serial.println("second tap");
  }
  LED1_state = !LED1_state;
  counter_press_LED1++;
}
void IRAM_ATTR press_LED2(){
  if(counter_press_LED2==0){
  fall_LED8= millis();
  Serial.println("first tap");
  }
  if(counter_press_LED2==1){
  fall2_LED8=millis();
  Serial.println("second tap");
  }
  LED2_state = !LED2_state;
  counter_press_LED2++;
}


