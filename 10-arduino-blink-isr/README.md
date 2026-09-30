# 10 · Blink with an interrupt

A pushbutton toggles an LED, but the button isn't polled in `loop()` — a hardware interrupt
fires on every edge. Each confirmed press is printed over Serial (115200 baud) together with
the gap since the previous one.

**Goal:** learn what may and may not happen inside an ISR, and why a timestamp alone is not
enough to debounce a mechanical button. The interrupt handler only stores a timestamp and
raises a flag; the decision and all the slow work happen back in `loop()`.

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Parts

- Tactile pushbutton
- LED + 220 Ω resistor
- 100 nF ceramic capacitor (optional but recommended)

## Wiring

| ESP32-S3 | Component |
|----------|-----------|
| GPIO4 | button leg 1 |
| GND | button leg 2, LED cathode |
| GPIO5 | 220 Ω resistor → LED anode |

No pull-up resistor is needed: `pinMode(BTN_PIN, INPUT_PULLUP)` enables the internal one, so
the pin idles HIGH and a press pulls it to GND.

Put the 100 nF capacitor across the button legs (GPIO4 to GND), in parallel with the button.
Together with the pull-up it forms an RC filter that rounds off the contact chatter before it
ever reaches the pin. The ESP32's internal pull-up is weak (~45 kΩ), which makes the rising
edge slow and the line easy to disturb; the capacitor is what turns a scratchy signal into one
clean transition. An external 10 kΩ pull-up from GPIO4 to 3V3 sharpens it further.

## How it works

1. `attachInterrupt(digitalPinToInterrupt(BTN_PIN), onEdge, CHANGE)` asks the GPIO peripheral to
   call `onEdge()` on every transition, press **and** release. Nothing in `loop()` polls the pin.
2. `onEdge()` does two assignments and returns: it stores `millis()` and sets `edgeSeen = true`.
   It decides nothing — deciding needs `Serial`, and `Serial` may not run here.
3. `loop()` waits until the line has been quiet for `DEBOUNCE_MS` (25 ms). Every new bounce
   refreshes `edgeTime`, so the timer restarts and the whole bounce train collapses into one
   event.
4. Then `loop()` reads the pin and compares it with the last confirmed state. The LED toggles
   only on a genuine released → pressed transition:
   `Press at 4192 ms, gap 1042 ms, LED ON`.

## Why the first version misbehaved

The original debounce lived in the ISR and looked only at time:

```cpp
if (now - lastTime > DEBOUNCE_MS) {   // 50 ms
  btnPressed = true;
  lastTime = now;
}
```

One press could toggle the LED twice, or seemingly not at all — two toggles in a row leave it
where it started. Three reasons:

- **The release was counted as a press.** The handler ran on `FALLING` and trusted every edge.
  Releasing the button chatters too, and that chatter arrives well after the 50 ms window has
  expired, so it was accepted as a brand new press.
- **A timestamp can't tell a bounce from a press.** A bounce train on a worn contact, fed
  through a weak 45 kΩ pull-up, can stretch past the window. Whatever the window, the code was
  guessing from timing instead of checking what the pin was actually doing.
- **Whoever came first won.** `lastTime` moved on the first accepted edge, so if a bounce got
  in first, the real press landed inside the dead window and was dropped.

The fix is to stop treating edges as the truth. **The pin level is the state; the interrupt is
only a hint that it's worth looking.** `loop()` waits for the line to settle, reads the pin, and
acts only when the level genuinely differs from the last confirmed one. A release can then only
ever set the state back to "released" — it can't toggle the LED. One press is exactly one
toggle, and no window tuning can break it.

## Code structure

- `src/main.cpp` — pins, the ISR, and the debounce state machine in `loop()`
- `include/Led.h`, `src/Led.cpp` — the `Led` class; `begin()` owns its `pinMode` and the
  initial state, so the pin is never left undefined

## What must not go inside an ISR

This project also exists because of one mistake that's easy to make: calling `Serial.printf()`
from the handler. It crashes the board with

```
Guru Meditation Error: Core 1 panic'ed (Interrupt wdt timeout on CPU1)
Core 1 was running in ISR context:
```

- **`Serial` here is USB CDC** (`ARDUINO_USB_CDC_ON_BOOT=1`), not a plain UART. It uses locks
  and a TX buffer that a *task* drains. When the buffer fills, `printf` blocks and waits — but
  inside an ISR the scheduler doesn't run, so no task can ever drain it. The interrupt watchdog
  (~300 ms) fires and the board panics.
- **An ISR has microseconds, not milliseconds.** While it runs, interrupts of the same and
  lower priority are blocked. Formatting a string and pushing it over USB is orders of
  magnitude too slow.
- **Bouncing multiplies it.** The burst calls the handler several times per millisecond, so
  even a fast handler adds up.

The rule: an ISR sets a flag and copies a value. Everything else — `Serial`, `delay()`,
`malloc`, I²C/SPI, anything blocking — belongs in `loop()`.

## Notes

- `ARDUINO_ISR_ATTR` is a section attribute that asks for the handler to be placed in IRAM
  instead of flash — but **only when `CONFIG_ARDUINO_ISR_IRAM` is enabled**, which it is not in
  the stock Arduino build (`esp32-hal.h` then expands the macro to nothing). That's consistent:
  the same config also controls `ARDUINO_ISR_FLAG`, the `ESP_INTR_FLAG_IRAM` that
  `attachInterrupt` passes to `gpio_install_isr_service()`. Without that flag, GPIO interrupts
  are simply masked while the flash cache is disabled, so a flash-resident handler is safe — it
  just doesn't fire during a flash write. Keep the attribute: it documents the intent, costs
  nothing, and makes the handler correct if the config is ever turned on.
- `edgeSeen` and `edgeTime` are `volatile`, otherwise the compiler may cache them in a register
  and `loop()` would never see the change. `btnPressed` and `lastPress` are touched only by
  `loop()`, so they don't need it.
- `btnPressed` is initialised from `digitalRead()` in `setup()`, not hardcoded, so booting with
  the button held down doesn't produce a phantom press.
- Clearing `edgeSeen` before reading the pin is deliberate: if a new edge sneaks in right after,
  the pin still reports the real level, and the next edge sets the flag again. The pin, not the
  flag, is the source of truth.
- `millis()` is safe in an ISR on the ESP32 — it reads `esp_timer`, which is in IRAM.
- `%lu`, not `%d`: `millis()` returns `unsigned long`. On a 32-bit ESP32 both happen to be the
  same width, so `%d` looks like it works, which makes it a good habit to get right early.
- `digitalPinToInterrupt()` returns the same number as the pin on the ESP32, but it's the
  portable Arduino way to write it.
- Holding the button down toggles the LED once, on the way down. Nothing happens on release.

## Try it

```sh
pio run -t upload -t monitor
```

Every press flips the LED, once, and prints one line. Mash the button as fast as you can — the
count of toggles should still match the count of presses.

## Local simulation

1. Build the project: `pio run`
2. In VS Code with the Wokwi extension: `F1` → **Wokwi: Start Simulator**
3. Click the button. Each click prints one line and flips the LED.

`wokwi.toml` points the simulator at the PlatformIO build output, and `diagram.json` describes
the circuit. The board has `"serialInterface": "USB_SERIAL_JTAG"` set, and nothing is connected
to `$serialMonitor`. The firmware sends `Serial` over native USB
(`ARDUINO_USB_CDC_ON_BOOT=1`), so a TX/RX connection would show nothing in the simulator.

The debounce can't actually be tested here: the Wokwi pushbutton is ideal and produces exactly
one clean edge per click, which is why the simulation has no capacitor either. The simulator is
useful for checking the logic and the log format — the bouncing only shows up on real hardware.
