#include "Thermistor.h"

#include <Arduino.h>

constexpr float SERIES_RESISTOR_OHMS = 10000.0f;
constexpr float NOMINAL_RESISTANCE_OHMS = 10000.0f;  // thermistor resistance at 25 °C
constexpr float NOMINAL_TEMPERATURE_K = 298.15f;     // 25 °C
constexpr float B_COEFFICIENT = 3950.0f;
constexpr float SUPPLY_MV = 3300.0f;

// The ESP32-S3 ADC reads up to about 3100 mV, so values near either end are not trusted
constexpr float MIN_VALID_MV = 50.0f;
constexpr float MAX_VALID_MV = 3050.0f;

constexpr int SAMPLES = 32;

Thermistor::Thermistor(uint8_t pin) : _pin(pin) {}

void Thermistor::begin() {
  pinMode(_pin, INPUT);
}

float Thermistor::readCelsius() {
  uint32_t sum_mv = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum_mv += analogReadMilliVolts(_pin);
    delayMicroseconds(200);
  }
  float mv = static_cast<float>(sum_mv) / SAMPLES;

  if (mv < MIN_VALID_MV || mv > MAX_VALID_MV) {
    return NAN;
  }

  float thermistor_ohms = SERIES_RESISTOR_OHMS * mv / (SUPPLY_MV - mv);
  // Beta equation: 1/T = 1/T0 + ln(R/R0) / B
  float kelvin = 1.0f / (1.0f / NOMINAL_TEMPERATURE_K +
                         logf(thermistor_ohms / NOMINAL_RESISTANCE_OHMS) / B_COEFFICIENT);
  return kelvin - 273.15f;
}
