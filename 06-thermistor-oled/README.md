# 06 · Thermistor thermometer on OLED

Measures temperature with a 10 kΩ NTC thermistor and shows it on a 128×64 SSD1306 OLED,
updated every 200 ms. At startup the I2C bus is scanned, the same way as in
[05-oled-i2c](../05-oled-i2c).

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Parts

- 10 kΩ NTC thermistor (B = 3950)
- 10 kΩ resistor
- SSD1306 128×64 I2C OLED

## Wiring

### Thermistor

The resistor and the thermistor form a voltage divider. GPIO1 reads the voltage between them.

```
3V3 ──[ 10 kΩ resistor ]──┬── GPIO1
                          │
                  [ NTC thermistor ]
                          │
GND ──────────────────────┘
```

| From | To |
|------|----|
| 3V3 | 10 kΩ resistor, first leg |
| 10 kΩ resistor, second leg | GPIO1 **and** thermistor, first leg |
| Thermistor, second leg | GND |

Neither part has polarity, so either leg can go either way.

### OLED

Same display as in 05, only the pins are different.

| OLED | ESP32-S3 |
|------|----------|
| VCC | 3V3 |
| GND | GND |
| SCL | GPIO5 |
| SDA | GPIO6 |

## How the temperature is calculated

1. Read the voltage on GPIO1 32 times and average it (`analogReadMilliVolts`).
2. Get the thermistor resistance from the divider: `R = 10 kΩ × V / (3300 mV − V)`.
3. Convert resistance to temperature with the Beta equation:
   `1/T = 1/T0 + ln(R / R0) / B`, where `T0` = 25 °C (298.15 K), `R0` = 10 kΩ, `B` = 3950.

When it gets warmer, the thermistor resistance drops, so the voltage on GPIO1 drops too.

## Code structure

```
main.cpp  →  Thermistor  →  analogReadMilliVolts()
          →  Display     →  U8g2
```

| File | Role |
|------|------|
| `include/Thermistor.h`, `src/Thermistor.cpp` | Reads the divider voltage and converts it to °C |
| `include/Display.h`, `src/Display.cpp` | Owns the U8g2 object and draws the temperature |
| `src/main.cpp` | Pins, I2C scan at startup, update loop |

## Notes

- `analogReadMilliVolts` is used instead of `analogRead`. The ESP32-S3 ADC reads only up to
  about 3100 mV, so a raw value of 4095 does not mean 3.3 V. Calibrated millivolts avoid an
  error of a few degrees.
- **`sensor error` on the screen** means the voltage is below 50 mV or above 3050 mV: a leg of the
  resistor or thermistor is loose or shorted. Temperatures below about −20 °C also land there.
- `B = 3950` is the most common value for 10 kΩ NTC thermistors. If yours differs, check its
  datasheet and change `B_COEFFICIENT` in `src/Thermistor.cpp`.
- Use ADC1 pins (GPIO1–GPIO10) for analog readings. ADC2 pins stop working while Wi-Fi is on.

## Try it

```sh
pio run -t upload
```
