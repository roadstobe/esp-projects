#include "LedAction.h"

#include <Arduino.h>

LedAction::LedAction(RgbLed& led) : _led(led) {}

void LedAction::blink(uint8_t times, Rgb color, uint16_t delayMs) {
  for (uint8_t i = 0; i < times; i++) {
    _led.setColor(color.red, color.green, color.blue);
    delay(delayMs);
    _led.off();
    delay(delayMs);
  }
}

void LedAction::pingPong() {
  for (uint8_t i = 0; i < 5; i++) {
    _led.setColor(233, 0, 170);
    delay(500);
    _led.setColor(0, 150, 200);
    delay(700);
  }
  _led.off();
}

void LedAction::turnOn(Rgb color) {
  _led.setColor(color.red, color.green, color.blue);
}

void LedAction::turnOff() {
  _led.off();
}
