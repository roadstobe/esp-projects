#include <esp32-hal-gpio.h>
#include <Led.h>


Led::Led(uint8_t pin): _pin(pin) {}

void Led::begin() {
  pinMode(_pin, OUTPUT);
  _state = LED_LIGHT::OFF;
  digitalWrite(_pin, _state);
}

void Led::switchLight() {
  _state = _state == LED_LIGHT::ON ? LED_LIGHT::OFF : LED_LIGHT::ON;
  digitalWrite(_pin, _state);
}

bool Led::isOn() const {
  return _state == LED_LIGHT::ON;
}
