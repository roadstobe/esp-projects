# 05 · OLED over I2C

Shows text typed into the serial monitor on a 128×64 SSD1306 OLED, inside a frame.
Uses the U8g2 library, so Ukrainian text works too. At startup the I2C bus is scanned and
every device found is printed over Serial.

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Wiring

| OLED | ESP32-S3 |
|------|----------|
| VCC | 3V3 |
| GND | GND |
| SCL | GPIO4 |
| SDA | GPIO5 |

The display answers at I2C address `0x3C`.

## Notes

- **Text position.** In `setCursor(x, y)`, `y` is the text baseline (the bottom of the letters),
  not the top. At `y = 0` the whole line is drawn above the screen and nothing is visible.
- **Ukrainian text** needs two things: `u8g2.enableUTF8Print()` after `begin()`, and a font
  with Cyrillic glyphs (`_t_cyrillic`). `_tr` fonts contain only ASCII. `u8g2_font_7x13_t_cyrillic`
  includes і, ї, є, ґ, but `u8g2_font_haxrcorp4089_t_cyrillic` does not.
- **Line length.** A 7 px wide font fits about 17 characters inside the frame. The rest is cut off.
- **Why scan the bus.** `u8g2.begin()` always returns `true`, even with no display connected.
  The scan shows whether the display is actually on the bus.
- The scan result is printed once at startup, before the monitor usually connects.
  Press **RST** with the monitor open to see it.
- A 1.3" module usually has an SH1106 controller instead of SSD1306. Use
  `U8G2_SH1106_128X64_NONAME_F_HW_I2C` with the same arguments.

## Try it

```sh
pio run -t upload -t monitor
```

Type `Привіт` and press Enter.

## Local simulation

1. Build the project: `pio run`
2. In VS Code with the Wokwi extension: `F1` → **Wokwi: Start Simulator**

`wokwi.toml` points the simulator at the PlatformIO build output, and `diagram.json` describes the circuit.
The board has `"serialInterface": "USB_SERIAL_JTAG"` set, and nothing is connected to
`$serialMonitor`. The firmware sends `Serial` over native USB (`ARDUINO_USB_CDC_ON_BOOT=1`),
so a TX/RX connection would show nothing in the simulator.
