#pragma once
#include <cstdint>

// 10 kΩ NTC thermistor in a voltage divider: 3V3 → 10 kΩ resistor → pin → thermistor → GND
class Thermistor {
public:
  explicit Thermistor(uint8_t pin);

  void begin();
  // Returns NAN when the voltage is out of range (a wire is loose or shorted)
  float readCelsius();

private:
  uint8_t _pin;
};
