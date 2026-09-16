#include <Arduino.h>
#include <Wire.h>

#include "Display.h"
#include "Thermistor.h"

constexpr uint8_t THERMISTOR_PIN = 1;
constexpr uint8_t OLED_SDA_PIN = 6;
constexpr uint8_t OLED_SCL_PIN = 5;

constexpr unsigned long UPDATE_INTERVAL_MS = 200;

Display display(OLED_SDA_PIN, OLED_SCL_PIN);
Thermistor thermistor(THERMISTOR_PIN);

bool scanI2C() {
  bool found = false;
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C device at 0x%02X\n", address);
      found = true;
    }
  }
  return found;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  if (!scanI2C()) {
    Serial.println("No I2C devices found, check wiring");
  }

  display.begin();
  thermistor.begin();
}

void loop() {
  float temperature = thermistor.readCelsius();
  display.showTemperature(temperature);
  delay(UPDATE_INTERVAL_MS);
}
