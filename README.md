# ESP Projects

A collection of small ESP32 projects I built while learning embedded development.
Each folder is a standalone PlatformIO project, so any of them can be copied and
used as a quick template for something new.

## Projects

| # | Project | What it does | Board |
|---|---------|--------------|-------|
| 01 | [button-led-debounce](01-button-led-debounce) | A button toggles an LED, with software debounce and state logging over Serial | ESP32-S3-DevKitC-1 (N16R8) |
| 02 | [potentiometer-adc](02-potentiometer-adc) | Reads a potentiometer via ADC and prints the raw value and percentage over Serial | ESP32-S3-DevKitC-1 (N16R8) |
| 03 | [serial-rgb-commands](03-serial-rgb-commands) | Controls the built-in RGB LED with text commands from the serial monitor, using classes | ESP32-S3-DevKitC-1 (N16R8) |
| 04 | [railway-crossing](04-railway-crossing) | Two LEDs blink in sync or alternately, mode switched by an external button and BOOT (`INPUT_PULLUP`) | ESP32-S3-DevKitC-1 (N16R8) |
| 05 | [oled-i2c](05-oled-i2c) | Shows text from the serial monitor on an SSD1306 OLED (U8g2, Ukrainian text) and scans the I2C bus | ESP32-S3-DevKitC-1 (N16R8) |
| 06 | [thermistor-oled](06-thermistor-oled) | Measures temperature with an NTC thermistor and shows it on an SSD1306 OLED, using classes | ESP32-S3-DevKitC-1 (N16R8) |
| 07 | [move-detection](07-move-detection) | A PIR sensor turns on an LED on motion, using a hardware interrupt and `millis()` timing | ESP32-S3-DevKitC-1 (N16R8) |

## Usage

Requires [PlatformIO](https://platformio.org/) (CLI or the IDE extension).

```sh
cd 01-button-led-debounce
pio run -t upload      # build and flash
pio device monitor     # open the serial monitor (115200 baud)
```

## Board setup: ESP32-S3 over the native USB port

Every project uses these build flags:

```ini
build_flags =
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
```

- `ARDUINO_USB_CDC_ON_BOOT=1` routes `Serial` to the USB port instead of the UART pins.
- `ARDUINO_USB_MODE=1` uses the chip's built-in USB-Serial/JTAG. The `esp32-s3-devkitc1-n16r8`
  board definition doesn't set it, so without the flag `Serial` runs on TinyUSB. In that mode,
  uploads need the BOOT button and `pio device monitor` shows no output (it opens the
  port with DTR off, and TinyUSB only sends data when DTR is on).

Output printed before the serial monitor is open is lost. To see the startup messages,
press **RST** on the board while the monitor is open. `pio device monitor` reconnects automatically.

## Starting a new project

Copy the closest existing project, rename it with the next number
(`02-something`, `03-something`, ...), and adjust `platformio.ini` and `src/main.cpp`.
