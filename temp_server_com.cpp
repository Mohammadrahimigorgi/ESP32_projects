#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_AHTX0.h>
#include <ArduinoJson.h>
#include <BH1750.h>
#include <Wire.h>

// Replace with your network credentials
const char* ssid = "Sharif-WiFi";
Adafruit_AHTX0 aht; 
//const char* password = "mamad1382";
long long int counter = 0;
// Server URL
const char* serverUrl = "http://172.27.10.192:5000/";
StaticJsonDocument<200> doc;
JsonArray tempArr = doc.createNestedArray("temp");
JsonArray humArr = doc.createNestedArray("hum");

void setup() {
  Serial.begin(115200);
  Wire.begin();
  aht.begin();
  // Connect to Wi-Fi
  WiFi.begin(ssid);
  // Create JSON document

  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    counter++;
    // Send HTTP GET request
    HTTPClient http;
    http.begin(serverUrl);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP GET Response code: " + String(httpResponseCode));
    } else {
      Serial.println("Error in HTTP GET request");
    }

    http.end(); // Free resources

    // Send HTTP POST request
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    sensors_event_t humidity, temper;
    aht.getEvent(&humidity, &temper);// populate temp and humidity objects with fresh data

    tempArr.add(temper.temperature);
    humArr.add(humidity.relative_humidity);
    if(counter%5== 0){

    String jsonString;
    serializeJson(doc, jsonString);

    httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP POST Response code: " + String(httpResponseCode));
      Serial.println("POST Response: " + response);
    } else {
      Serial.println("Error in HTTP POST request");
    }

    tempArr.clear();
    humArr.clear();

    }
    

    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Wait for 1 seconds before sending the next requests
  delay(1000);
  
}
