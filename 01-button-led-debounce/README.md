# 01 · Button + LED with debounce

A button turns an LED on while pressed. The input is debounced in software,
and every state change is logged over Serial (115200 baud).

**Board:** ESP32-S3-DevKitC-1 (N16R8)
**Simulation:** [Wokwi project](https://wokwi.com/projects/475125311468334081)

## Wiring

| ESP32-S3 | Component |
|----------|-----------|
| GPIO1 | 220 Ω resistor → LED anode, LED cathode → GND |
| GPIO4 | Button → 3V3, plus a pull-down resistor → GND |

The button pin uses `INPUT` without an internal pull resistor, so the external
pull-down is required: released = `LOW`, pressed = `HIGH`.

## Local simulation

1. Build the project: `pio run`
2. In VS Code with the Wokwi extension: `F1` → **Wokwi: Start Simulator**

`wokwi.toml` points the simulator at the PlatformIO build output, and `diagram.json` describes the circuit.
