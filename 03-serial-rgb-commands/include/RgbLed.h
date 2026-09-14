#pragma once

#include <Arduino.h>

class RgbLed {
public:
  RgbLed(uint8_t pin);

  void begin();
  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  void setBrightness(uint8_t brightness);
  void off();

private:
  void apply();

  uint8_t _pin;
  uint8_t _brightness = 30;
  uint8_t _red = 0;
  uint8_t _green = 0;
  uint8_t _blue = 0;
};
