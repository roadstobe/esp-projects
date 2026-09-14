#include <Arduino.h>

constexpr uint8_t LED = 1;
constexpr uint8_t BTN = 4;

int debounceDelay = 500;

int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;



void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED, OUTPUT);
  pinMode(BTN, INPUT);
}

void loop() {
  int value = digitalRead(BTN);

  if (value != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
  if (value != buttonState) {
    buttonState = value;
    Serial.printf("[%lu] button %s\n", millis(), buttonState ? "PRESSED" : "RELEASED");
    digitalWrite(LED, buttonState);
  }
}

  lastButtonState = value;
}

