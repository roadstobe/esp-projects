#include <Arduino.h>

constexpr uint8_t POTENTIOMETER = 4;

void setup() {
    Serial.begin(115200);
}

void loop() {
    int value = analogRead(POTENTIOMETER);
    int valueInPercentage = map(value, 0, 4095, 0, 100);
    Serial.printf("%d (%d%%)\n", value, valueInPercentage);
    delay(100);
}