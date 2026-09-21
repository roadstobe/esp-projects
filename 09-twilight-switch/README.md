# 09 · Twilight switch

A photoresistor (LDR) measures ambient light, and a relay turns a lamp on at dusk and off
at dawn. An LED stands in for the lamp. The light level and lamp state are printed over
Serial (115200 baud).

**Goal:** turn a noisy analog reading into stable on/off switching (averaging, hysteresis,
a minimum time between switches) and drive a 5 V relay module from a 3.3 V GPIO through a
transistor.

**Board:** ESP32-S3-DevKitC-1 (N16R8)

## Parts

- LDR (photoresistor) module with an analog output (AO)
- 5 V single-channel relay module, low-level trigger (IN = LOW turns it on)
- BC547 NPN transistor
- 2 × 10 kΩ resistors
- LED + 220 Ω resistor

## Wiring

| ESP32-S3 | Component |
|----------|-----------|
| 3V3 | LDR VCC |
| GND | LDR GND, BC547 emitter, relay GND, LED cathode |
| GPIO1 (ADC1_CH0) | LDR AO |
| GPIO5 | 10 kΩ resistor → BC547 base |
| 5V | relay VCC, relay COM, 10 kΩ pull-up → BC547 collector |

- BC547 collector → relay IN (together with the 10 kΩ pull-up to 5V)
- Relay NO → 220 Ω resistor → LED anode

Power the LDR from **3V3, never 5V**: the ADC pin tolerates at most 3.3 V.
BC547 pinout, flat side facing you, legs down: **C B E** (collector, base, emitter).

### Why the transistor

The relay module runs on 5 V and turns on when IN is pulled LOW. A 3.3 V GPIO can't pull
IN all the way up to 5 V, so the relay may not release. The BC547 works as a switch in
between:

- GPIO5 HIGH → the transistor conducts → IN is pulled to GND → relay on
- GPIO5 LOW → the transistor is off → the 10 kΩ resistor pulls IN up to 5 V → relay off

This also means HIGH = on in the code. While the board boots and GPIO5 isn't configured
yet, the relay stays off.

## How it works

1. `readLightLevel()` averages 16 ADC readings (the ESP32 ADC is noisy) and returns a light
   level from 0 to 4095, where higher means brighter.
2. **Hysteresis.** The lamp turns on when the level drops below `ON_BELOW` (800) and turns off
   only when it rises above `OFF_ABOVE` (1200). With a single threshold, the reading hovers
   around it at dusk and the relay would keep clicking on and off.
3. **Minimum switch interval.** After each switch the lamp keeps its state for at least
   `MIN_SWITCH_MS` (2 s), so a passing shadow or car headlights can't toggle it.
4. Every 200 ms the level and the lamp state are printed: `Light:  612 | Lamp: ON`.

## Notes

- **Sensor direction.** Whether the reading goes up or down in the dark depends on which side
  of the voltage divider the LDR is on. On my module it goes down. The Wokwi module works the
  other way round, so the `wokwi` build environment adds `-DLDR_DARK_IS_HIGH=1`, and
  `readLightLevel()` flips the value. The rest of the code always sees "higher = brighter".
  If your module reads higher in the dark, add the same flag to the main environment.
- **Tuning.** Watch `Light:` in the serial monitor when it's as dark as you want the lamp
  to turn on. Set `ON_BELOW` a little below that value and `OFF_ABOVE` a few hundred above it.
- Point the sensor away from the lamp. Otherwise the lamp lights up the sensor and turns itself off.
- The LED stands in for a real lamp. A relay module can switch mains, but never wire mains on
  a breadboard. Use a module rated for the load, inside an enclosure.

## Try it

```sh
pio run -t upload -t monitor
```

Cover the sensor with your hand, and the relay clicks and the LED lights up. Uncover it,
and the LED turns off again (no sooner than 2 s after it turned on).

## Local simulation

1. Compile the BC547 custom chip once (needs the Wokwi CLI):
   `wokwi-cli chip compile chips/bc547.chip.c -o chips/bc547.chip.wasm`
2. Build the Wokwi variant of the firmware: `pio run -e wokwi`
3. In VS Code with the Wokwi extension: `F1` → **Wokwi: Start Simulator**
4. Click the photoresistor and drag the lux slider down until `Light:` drops below 800.

`wokwi.toml` points the simulator at the `wokwi` build output and loads the custom chip, and
`diagram.json` describes the circuit. The BC547 is a custom chip (`chips/bc547.chip.c`)
that works as a simple switch: when the base is HIGH, it pulls the collector to GND; otherwise
the collector floats.

The board has `"serialInterface": "USB_SERIAL_JTAG"` set, and nothing is connected to
`$serialMonitor`. The firmware sends `Serial` over native USB (`ARDUINO_USB_CDC_ON_BOOT=1`),
so a TX/RX connection would show nothing in the simulator.

The same circuit is also on [Wokwi online](https://wokwi.com/projects/475754591916054529),
where it runs in the browser without a local build.
