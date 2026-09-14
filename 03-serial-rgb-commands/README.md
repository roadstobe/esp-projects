# 03 · Serial RGB commands

Controls the board's built-in RGB LED (GPIO48) with text commands typed into the serial monitor.
The logic is split into small classes as practice in structuring code across files.

**Board:** ESP32-S3-DevKitC-1 (N16R8), no extra wiring.

## Commands

| Command | What happens |
|---------|--------------|
| `blink` | Blinks green once |
| `double` | Blinks green twice |
| `on` | Turns the LED blue |
| `off` | Turns the LED off |
| `ping` | Alternates magenta and cyan 5 times, then turns off |
| anything else | Echoed back to the monitor |

## Code structure

```
main.cpp  →  LedAction  →  RgbLed  →  rgbLedWrite()
(commands)   (effects)     (color,      (hardware)
                           brightness)
```

| File | Role |
|------|------|
| `include/Rgb.h` | `struct Rgb`: a color as three numbers |
| `include/RgbLed.h`, `src/RgbLed.cpp` | Owns the pin and brightness. The only place that calls `rgbLedWrite` |
| `include/LedAction.h`, `src/LedAction.cpp` | Effects (blink, ping-pong, on/off). Holds a reference to an `RgbLed` |
| `src/main.cpp` | Reads lines from Serial and maps commands to actions |

## Serial monitor settings

The monitor options in `platformio.ini` make typing comfortable:

| Option | Why |
|--------|-----|
| `monitor_echo = yes` | Shows what you type |
| `monitor_filters = send_on_enter` | Sends the whole line only when you press Enter |
| `monitor_eol = LF` | Ends the line with `\n` only, without `\r` |

## Try it

```sh
pio run -t upload -t monitor
```

Type `blink` or `ping` and press Enter.
