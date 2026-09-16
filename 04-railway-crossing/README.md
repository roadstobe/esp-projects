# 04 · Railway crossing

Two LEDs blink in one of two modes, selected by two buttons. The chosen mode stays active
until the other button is pressed.

**Goal:** practice configuring digital pins as inputs and outputs, and using the built-in
pull-up (`INPUT_PULLUP`).

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Wiring

| ESP32-S3 | Component |
|----------|-----------|
| GPIO15 | 220 Ω resistor → LED 1 anode, cathode → GND |
| GPIO16 | 220 Ω resistor → LED 2 anode, cathode → GND |
| GPIO21 | Push button → GND (no resistor, `INPUT_PULLUP`) |
| GPIO0 | On-board **BOOT** button (pull-up resistor already on the board, `INPUT`) |

Both buttons are active LOW: released reads `HIGH`, pressed reads `LOW`.

## Modes

| Mode | Selected by | Effect |
|------|-------------|--------|
| 1 · Sync (default at startup) | External button (GPIO21) | Both LEDs on and off together, every 200 ms |
| 2 · Alternate | BOOT button (GPIO0) | LEDs take turns, like a railway crossing, every 1000 ms |

## Timing with `millis()` instead of `delay()`

`loop()` never waits. On every pass it reads both buttons, then checks whether the current
interval has passed since the last LED switch:

```cpp
unsigned long now = millis();
if (now - last_step_ms >= currentInterval()) {
  last_step_ms = now;
  phase = !phase;
  applyPhase();
}
```

Because `loop()` runs thousands of times per second, a button press switches the mode immediately.

## Code structure

`src/main.cpp` currently holds the whole program as plain functions. The class skeleton below is
kept for moving the logic into classes later:

```
main.cpp (buttons)  →  RailwayCrossing (mode, blinking)  →  Led  →  digitalWrite()
```

| File | Role |
|------|------|
| `include/BlinkMode.h` | `enum class BlinkMode { Sync, Alternate }` |
| `include/Led.h`, `src/Led.cpp` | One LED on a digital output |
| `include/Button.h`, `src/Button.cpp` | One active-LOW button, pin mode passed in the constructor |
| `include/RailwayCrossing.h`, `src/RailwayCrossing.cpp` | Holds both LEDs and the current mode, blinks one half-cycle per `update()` |

## Notes

- The mode is only switched when it actually changes, so button bounce doesn't matter here.
- GPIO0 is a strapping pin. Holding BOOT while the board resets or powers up starts the
  bootloader instead of the program. Press it only while the program is running.
