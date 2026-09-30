#pragma once
#include <cstdint>

enum LED_LIGHT {
  OFF,
  ON
};

class Led {
public:
  Led(uint8_t pin);

  void begin();
  void switchLight();
  bool isOn() const;

private:
  uint8_t _pin;
  LED_LIGHT _state = LED_LIGHT::OFF;
};
