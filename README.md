# ESP Projects

A collection of small ESP32 projects I built while learning embedded development.
Each folder is a standalone PlatformIO project, so any of them can be copied and
used as a quick template for something new.

## Projects

| # | Project | What it does | Board |
|---|---------|--------------|-------|
| 01 | [button-led-debounce](01-button-led-debounce) | A button toggles an LED, with software debounce and state logging over Serial | ESP32-S3-DevKitC-1 (N16R8) |
| 02 | [potentiometer-adc](02-potentiometer-adc) | Reads a potentiometer via ADC and prints the raw value and percentage over Serial | ESP32-S3-DevKitC-1 (N16R8) |

## Usage

Requires [PlatformIO](https://platformio.org/) (CLI or the IDE extension).

```sh
cd 01-button-led-debounce
pio run -t upload      # build and flash
pio device monitor     # open the serial monitor (115200 baud)
```

## Starting a new project

Copy the closest existing project, rename it with the next number
(`02-something`, `03-something`, ...), and adjust `platformio.ini` and `src/main.cpp`.
