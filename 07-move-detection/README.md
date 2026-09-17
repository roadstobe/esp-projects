# 07 · Motion detection

A PIR sensor turns on an LED when it detects motion. The LED stays on while motion continues
and turns off 5 s after it stops. Events are logged over Serial (115200 baud).

**Goal:** practice hardware interrupts (`attachInterrupt`, `IRAM_ATTR`, `volatile`) together
with non-blocking timing on `millis()`.

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Parts

- HC-SR501 PIR motion sensor
- LED
- 220 Ω resistor

## Wiring

| ESP32-S3 | Component |
|----------|-----------|
| 5V | PIR VCC |
| GND | PIR GND, LED cathode |
| GPIO4 | PIR OUT |
| GPIO5 | 220 Ω resistor → LED anode |

The HC-SR501 needs 5 V to run, but its OUT pin gives 3.3 V, so it can go straight to GPIO4.
The DevKitC 5V pin has power only when the board is powered over USB.

## How it works

1. `setup()` waits 60 s (`PIR_WARMUP_MS`) while the sensor warms up. OUT can jump randomly
   during that time, so the interrupt is attached only afterwards.
2. The interrupt on the rising edge of OUT sets `motionFlag`. The ISR ignores edges less than
   200 ms apart and does nothing else: no `Serial`, no `digitalWrite`.
3. `loop()` treats `motionFlag` **or** a HIGH level on OUT as motion. Each time, it moves the
   turn-off time `LIGHT_HOLD_MS` ahead and turns the LED on if it is off.
4. When that time passes, the LED turns off.

Why the level is checked too: in retrigger mode the sensor keeps OUT HIGH as long as it sees
motion. There is only one rising edge, so with the interrupt alone the LED would turn off 5 s
after the first movement even while someone is still moving.

## Notes

- **HC-SR501 settings.** The *Tx* potentiometer sets how long OUT stays HIGH (from a few seconds to about 5 min),
  and *Sx* sets the range (about 3–7 m). The jumper selects the mode: **H** retriggers while
  motion continues, **L** gives one pulse per detection. The LED stays on for the OUT time plus 5 s.
- After OUT goes LOW, the sensor ignores motion for about 2.5 s.
- `delay()` in `setup()` is fine: it runs once, before `loop()` starts. `loop()` itself never waits.
- The startup messages are printed before the serial monitor usually connects.
  Press **RST** with the monitor open to see them (the warm-up starts again).

## Try it

```sh
pio run -t upload -t monitor
```

After `Ready`, wave your hand in front of the sensor.

## Local simulation

1. Build the project: `pio run`
2. In VS Code with the Wokwi extension: `F1` → **Wokwi: Start Simulator**
3. Wait for `Ready` (60 s of simulated time), click the PIR sensor and choose **Simulate motion**.

`wokwi.toml` points the simulator at the PlatformIO build output, and `diagram.json` describes the circuit.
The board has `"serialInterface": "USB_SERIAL_JTAG"` set, and nothing is connected to
`$serialMonitor`. The firmware sends `Serial` over native USB (`ARDUINO_USB_CDC_ON_BOOT=1`),
so a TX/RX connection would show nothing in the simulator.

To skip the wait in the simulator, set `PIR_WARMUP_MS` to `0` temporarily.
In Wokwi the sensor keeps OUT HIGH for 5 s after each motion.
