#include "RgbLed.h"

RgbLed::RgbLed(uint8_t pin) : _pin(pin) {}

void RgbLed::begin() {
  off();
}

void RgbLed::setColor(uint8_t red, uint8_t green, uint8_t blue) {
  _red = red;
  _green = green;
  _blue = blue;
  apply();
}

void RgbLed::setBrightness(uint8_t brightness) {
  _brightness = brightness;
  apply();
}

void RgbLed::off() {
  setColor(0, 0, 0);
}

void RgbLed::apply() {
  rgbLedWrite(_pin,
              _red * _brightness / 255,
              _green * _brightness / 255,
              _blue * _brightness / 255);
}
