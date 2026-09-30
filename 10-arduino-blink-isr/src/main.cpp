#include <Arduino.h>

#include "Led.h"


constexpr uint8_t  BTN_PIN     = 4;
constexpr uint8_t  LED_PIN     = 5;
constexpr uint32_t DEBOUNCE_MS = 25;

volatile bool     edgeSeen = false;
volatile uint32_t edgeTime = 0;

bool     btnPressed = false;
uint32_t lastPress  = 0;

Led led(LED_PIN);

void ARDUINO_ISR_ATTR onEdge();

void setup() {
  Serial.begin(115200);

  pinMode(BTN_PIN, INPUT_PULLUP);

  led.begin();

  btnPressed = digitalRead(BTN_PIN) == LOW;

  attachInterrupt(digitalPinToInterrupt(BTN_PIN), onEdge, CHANGE);
}

void ARDUINO_ISR_ATTR onEdge() {
  edgeTime = millis();
  edgeSeen = true;
}

void loop() {
  bool settled = edgeSeen && millis() - edgeTime >= DEBOUNCE_MS;

  if (!settled) {
    return;
  }

  edgeSeen = false;

  bool pressed = digitalRead(BTN_PIN) == LOW;

  if (pressed == btnPressed) {
    return;
  }

  btnPressed = pressed;

  if (!pressed) {
    return;
  }

  uint32_t now = millis();

  led.switchLight();

  Serial.printf("Press at %lu ms, gap %lu ms, LED %s\n",
                now, now - lastPress, led.isOn() ? "ON" : "OFF");

  lastPress = now;
}
