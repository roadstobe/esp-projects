#pragma once

#include "Rgb.h"
#include "RgbLed.h"

class LedAction {
public:
  LedAction(RgbLed& led);

  void blink(uint8_t times, Rgb color, uint16_t delayMs = 300);

  void pingPong();

  void turnOn(Rgb color);

  void turnOff();

private:
  RgbLed& _led;
};
