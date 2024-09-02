#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>


void saveDurationsToJson(const char* key, uint16_t array[], int length) ;
void loadDurationsFromJson(const char* key, uint16_t array[], int &length);
const int irPin = 5; // GPIO 19 for IR signal reception
uint16_t durations[280]; // Array to store durations with 243 elements
int counter = -1;
unsigned long lastSignalTime = 0;
unsigned long startTime = 0;
String rawdataToBinary(uint16_t array[]);
String data_bitStream;

void setup() {
  Serial.begin(115200);
  pinMode(irPin, INPUT);
    if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  Serial.println("Manual IR Receiver is ready to receive signals on port 19");
}

void loop() {
  unsigned long currentMillis = millis();
  bool lastState = digitalRead(irPin);
  bool currentState;

  while (true) {
    currentMillis = millis();
    currentState = digitalRead(irPin);
    // Detect state change
    if (currentState != lastState) {
      unsigned long duration = micros() - startTime; // Calculate the duration of the previous state
      startTime = micros(); // Reset the start time for the current state
      lastSignalTime = currentMillis; // Update the last signal time
      // Store the duration in the array, converted to uint16_t
      if(counter!=-1){
      durations[counter] = (uint16_t)(duration);
      }
      // Print the duration
      counter++;
      // Update the last state
      lastState = currentState;
    }
if(counter !=-1){
    // Clear the array after 3 seconds of inactivity
    if (currentMillis - lastSignalTime >= 3000) {
      Serial.println("");

      Serial.println("");
      data_bitStream = rawdataToBinary(durations);
      Serial.print("counter is : ");
      Serial.println(counter);
      saveDurationsToJson("on", durations, counter);  
      memset(durations, 0, sizeof(durations)); // Clear the array
      counter = -1; // Reset counter
      data_bitStream="";
      lastSignalTime = currentMillis; // Reset the last signal time to prevent continuous clearing
      Serial.println("Array cleared due to inactivity");
    }
  }


  
  }
}



String rawdataToBinary(uint16_t array[]){
String buffer ="";

for (int i =0 ; i < counter  ; i+=1){

 Serial.print(array[i]);
//\Serial.print(" with index of ");
Serial.print(",");

}
Serial.println("");
return buffer ;

}

void saveDurationsToJson(const char* key, uint16_t array[], int length) {
  // Create a JSON document to hold the existing data
  StaticJsonDocument<4096> doc;

  // Open the existing file, if it exists
  File file = LittleFS.open("/durations.json", "r");
  if (file) {
    // Deserialize the existing JSON data
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println("Failed to read file, using empty JSON document");
    }
    file.close();
  }

  // Create a new JSON array for the key
  JsonArray data = doc.createNestedArray(key);

  // Populate the JSON array with the new durations
  for (int i = 0; i < length; i++) {
    data.add(array[i]);
  }

  // Open the file for writing (this will overwrite the file)
  file = LittleFS.open("/durations.json", "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  // Serialize the updated JSON to the file
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to file");
  }

  file.close();
  Serial.println("Durations saved to LittleFS as JSON");
}


void loadDurationsFromJson(const char* key, uint16_t array[], int &length) {
  File file = LittleFS.open("/durations.json", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  // Create a JSON document to hold the data
  StaticJsonDocument<4096> doc;

  // Deserialize the JSON from the file
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.print("Failed to read file: ");
    Serial.println(error.c_str());
    return;
  }

  // Get the JSON array for the specified key
  JsonArray data = doc[key];

  // Copy the JSON array to the durations array
  length = data.size();
  for (int i = 0; i < length; i++) {
    array[i] = data[i].as<uint16_t>();
  }

  file.close();
  Serial.println("Durations loaded from LittleFS");
}