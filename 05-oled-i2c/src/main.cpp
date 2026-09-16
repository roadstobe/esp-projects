#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

constexpr uint8_t OLED_SDA_PIN = 5;
constexpr uint8_t OLED_SCL_PIN = 4;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL_PIN, OLED_SDA_PIN);

bool scanI2C() {
  bool found = false;
  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C device at 0x%02X\n", address);
      found = true;
    }
  }
  return found;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  if (!scanI2C()) {
    Serial.println("No I2C devices found, check wiring");
  }

  u8g2.begin();
  u8g2.enableUTF8Print();
}

void loop() {
  u8g2.setFont(u8g2_font_7x13_t_cyrillic);
  if (Serial.available() > 0) {
    String line = Serial.readStringUntil('\n');
    line.trim();

    u8g2.clearBuffer();
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setCursor(5, 20);
    u8g2.print(line);
    u8g2.sendBuffer();
  }

  delay(200);
}
