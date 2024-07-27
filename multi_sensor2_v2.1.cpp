#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <HTTPClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <Adafruit_AHTX0.h>

void initLittleFS(); // littleFs initilize

bool initWiFi();

void serverConnect();

void IRAM_ATTR detectsMovement();

// Replace with your network credentials
const int motionSensor = 14;
const int led = 27;
boolean motion=false;

unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

int counter_wifi=0;

StaticJsonDocument<200> docTH;
Adafruit_AHTX0 aht; 
long long int counter = 0;



boolean reco = true;
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


boolean action ; // true if wifi is connected and ESP is connected to server

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

StaticJsonDocument<200> doc;



// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";

//Variables to save values from HTML form
String ssid;
String pass;
String ip;


// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";

IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded

IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)






// put function declarations here:

void setup() {
  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
  Serial.begin(115200);
  Wire.begin();
  aht.begin();

  initLittleFS();

  ssid = readFile(LittleFS, ssidPath);
  pass = readFile(LittleFS, passPath);
  ip = "http://"+readFile(LittleFS, ipPath);
  Serial.println("SSID : " + ssid);
  Serial.println("Password : " +pass);
  Serial.println("IP : " +ip);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str());
  // Create JSON document

  Serial.print("Connecting to Wi-Fi...");
  while ((WiFi.status() != WL_CONNECTED) && reco) {
    delay(300);
    Serial.print(".");
    counter_wifi++;
    if(counter_wifi>100){
      reco = false;
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
  Serial.println("connected to wifi : " + ssid);
  Serial.println("IP server : " + ip);
  HTTPClient http;
  http.begin(ip);
  int httpResponseCode = http.GET();
  String response = http.getString();
  Serial.println(httpResponseCode);
    if(httpResponseCode>0){
  Serial.print("response of server is : ");
  Serial.println(response);  
  Serial.println("end http");
  ArduinoOTA.begin();
  Serial.print("IP Adrress : ");
  Serial.println(WiFi.localIP());
  action = true;
  ArduinoOTA.begin();
  Serial.print("IP Adrress : ");
  Serial.println(WiFi.localIP());
    }
    else{
    serverConnect();
    Serial.println("IP address not valid");
    ArduinoOTA.begin();
    Serial.print("IP Adrress : ");
    Serial.println(WiFi.localIP());
    }
  }
  else {
    serverConnect();
    Serial.println("wifi or password not valid");
}




}

void loop() {
  ArduinoOTA.handle();
  now = millis();
  if (WiFi.status() == WL_CONNECTED) {
    counter++;
    // Send HTTP GET request
    HTTPClient http;
    http.begin(ip);


    // Send HTTP POST request
    http.begin(ip);
    http.addHeader("Content-Type", "application/json");
    sensors_event_t humidity, temper;
    aht.getEvent(&humidity, &temper);// populate temp and humidity objects with fresh data

    if(counter%5== 0){
    docTH["temp"] = (int)(round(temper.temperature*100.0))/100.0;
    docTH["hum"]= (int)(round(humidity.relative_humidity*100.0))/100.0;
    docTH["motion"] = motion; 
    String jsonString;
    serializeJson(docTH, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP POST Response code: " + String(httpResponseCode));
     // Serial.println("POST Response: " + response);
    } else {
      Serial.println("Error in HTTP POST request");
    }

    counter = 0;
    motion = false;
    }
    

    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Wait for 1 seconds before sending the next requests
  delay(1000);
  
}







void IRAM_ATTR detectsMovement() {
  motion = true;
  Serial.println("motion detected");
  startTimer = true;
  lastTrigger = millis();
}



void serverConnect(){

    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("ESP-Sensor2", NULL);

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





// Initialize WiFi









// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}







