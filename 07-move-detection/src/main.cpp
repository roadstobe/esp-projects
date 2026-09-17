#include <Arduino.h>

constexpr uint8_t PIR_PIN = 4;
constexpr uint8_t LED_PIN = 5;
constexpr uint32_t PIR_WARMUP_MS = 60000;
constexpr uint32_t LIGHT_HOLD_MS = 5000;

volatile bool motionFlag = false;
volatile uint32_t lastIsrMs = 0;

uint32_t lightOffAt = 0;
bool lightOn = false;

void IRAM_ATTR onMotion() {
  uint32_t now = millis();
  if (now - lastIsrMs < 200) return;

  lastIsrMs = now;
  motionFlag = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("PIR warming up, 60 s...");
  delay(PIR_WARMUP_MS);
  Serial.println("Ready");

  attachInterrupt(digitalPinToInterrupt(PIR_PIN), onMotion, RISING);
}

void loop() {
  uint32_t now = millis();

  // RISING fires only once per pulse. A retriggering PIR keeps OUT high while
  // motion continues, so a high level also counts as motion.
  if (motionFlag || digitalRead(PIR_PIN) == HIGH) {
    motionFlag = false;
    lightOffAt = now + LIGHT_HOLD_MS;

    if (!lightOn) {
      lightOn = true;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("motion -> light on");
    }
  }

  if (lightOn && (int32_t)(now - lightOffAt) >= 0) {
    lightOn = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("light off");
  }
}
