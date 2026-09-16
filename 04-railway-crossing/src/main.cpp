#include <Arduino.h>

constexpr uint8_t LED_RED_PIN = 15;
constexpr uint8_t LED_BLUE_PIN = 16;
constexpr uint8_t BTN_PIN = 21;
constexpr uint8_t BOOT_BUTTON_PIN = 0;

constexpr unsigned long SYNC_INTERVAL_MS = 200;
constexpr unsigned long ALTERNATE_INTERVAL_MS = 1000;

enum class BlinkMode {
  Sync,
  Alternate,
};

BlinkMode blink_mode = BlinkMode::Sync;

bool first_state = true;

unsigned long last_step_ms = 0;

void applyPhase() {
  if (blink_mode == BlinkMode::Sync) {
    digitalWrite(LED_RED_PIN, first_state ? HIGH : LOW);
    digitalWrite(LED_BLUE_PIN, first_state ? HIGH : LOW);
  } else {
    digitalWrite(LED_RED_PIN, first_state ? HIGH : LOW);
    digitalWrite(LED_BLUE_PIN, first_state ? LOW : HIGH);
  }
}

unsigned long currentInterval() {
  return blink_mode == BlinkMode::Sync ? SYNC_INTERVAL_MS : ALTERNATE_INTERVAL_MS;
}

void setMode(BlinkMode new_mode) {
  if (new_mode == blink_mode) {
    return;
  }

  blink_mode = new_mode;

  first_state = true;
  applyPhase();
  last_step_ms = millis();

  Serial.printf("Mode: %s\n", blink_mode == BlinkMode::Sync ? "Sync" : "Alternate");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BOOT_BUTTON_PIN, INPUT);

  applyPhase();
  last_step_ms = millis();
}

void loop() {
  if (digitalRead(BTN_PIN) == LOW) {
    setMode(BlinkMode::Sync);
  }
  if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
    setMode(BlinkMode::Alternate);
  }

  unsigned long now = millis();
  if (now - last_step_ms >= currentInterval()) {
    last_step_ms = now;
    first_state = !first_state;
    applyPhase();
  }
}
