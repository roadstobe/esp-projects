#include <Arduino.h>

#ifndef LDR_DARK_IS_HIGH
#define LDR_DARK_IS_HIGH 0
#endif

constexpr uint8_t LDR_PIN = 1;
constexpr uint8_t RELAY_PIN = 5;

constexpr int ADC_MAX = 4095;
constexpr int ON_BELOW = 800;
constexpr int OFF_ABOVE = 1200;
constexpr uint8_t SAMPLES = 16;
constexpr uint32_t MIN_SWITCH_MS = 2000;
constexpr uint32_t READ_INTERVAL_MS = 200;

bool lampOn = false;
uint32_t lastSwitchMs = 0;

int readLightLevel() {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < SAMPLES; i++) {
    sum += analogRead(LDR_PIN);
    delay(2);
  }
  int raw = sum / SAMPLES;
  return LDR_DARK_IS_HIGH ? ADC_MAX - raw : raw;
}

void setLamp(bool on) {
  lampOn = on;
  lastSwitchMs = millis();
  digitalWrite(RELAY_PIN, on ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(RELAY_PIN, OUTPUT);
  setLamp(false);
}

void loop() {
  int level = readLightLevel();
  bool canSwitch = millis() - lastSwitchMs >= MIN_SWITCH_MS;

  if (canSwitch) {
    if (!lampOn && level < ON_BELOW) {
      setLamp(true);
    } else if (lampOn && level > OFF_ABOVE) {
      setLamp(false);
    }
  }

  Serial.printf("Light: %4d | Lamp: %s\n", level, lampOn ? "ON" : "OFF");
  delay(READ_INTERVAL_MS);
}
