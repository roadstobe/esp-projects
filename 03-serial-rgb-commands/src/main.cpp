#include <Arduino.h>
#include "LedAction.h"
#include "Rgb.h"
#include "RgbLed.h"

RgbLed led(RGB_BUILTIN);
LedAction actions(led);

void inputHandler(const String& action);

void setup() {
    Serial.begin(115200);
    led.begin();
    led.setBrightness(30);
}

void loop() {
    if (Serial.available() > 0) {
        String line = Serial.readStringUntil('\n');
        line.trim();

        if (line.length() == 0) {
            return;
        }

        inputHandler(line);
    }
}

void inputHandler(const String& action) {
    if (action == "blink") {
        actions.blink(1, Rgb{0, 255, 0});
    } else if (action == "double") {
        actions.blink(2, Rgb{0, 255, 0});
    } else if (action == "on") {
        actions.turnOn(Rgb{0, 0, 150});
    } else if (action == "off") {
        actions.turnOff();
    } else if (action == "ping") {
        actions.pingPong();
    } else {
        Serial.println(action);
    }
}
