#include <Adafruit_AHTX0.h>
#include <Wire.h>
#include <BH1750.h>


BH1750 lightMeter;


unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
}


void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
}