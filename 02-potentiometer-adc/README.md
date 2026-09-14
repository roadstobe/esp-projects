# 02 · Potentiometer ADC read

Reads a potentiometer with the ESP32-S3 ADC and prints the raw value and
percentage over Serial (115200 baud) every 100 ms.

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Wiring

| Potentiometer | ESP32-S3 |
|---------------|----------|
| VCC (outer pin) | 3V3 |
| SIG (middle pin) | GPIO4 (ADC1_CH3) |
| GND (outer pin) | GND |

Power the potentiometer from **3V3, never 5V**: the ADC pin tolerates at most 3.3 V.

## Notes

- `analogRead` returns 12 bits by default, so values range from 0 to 4095.
- Readings jitter a little even when the knob isn't moving. The ESP32 ADC is noisy.
  To improve this, add a 100 nF capacitor between GPIO4 and GND, or average several samples in code.

## Local simulation

1. Build the project: `pio run`
2. In VS Code with the Wokwi extension: `F1` → **Wokwi: Start Simulator**

`wokwi.toml` points the simulator at the PlatformIO build output, and `diagram.json` describes the circuit.
