#include "Display.h"

constexpr uint8_t WIDTH  = 128;
constexpr uint8_t HEIGHT = 64;

Display::Display(uint8_t sdaPin, uint8_t sclPin)
  : _u8g2(U8G2_R0, U8X8_PIN_NONE, sclPin, sdaPin) {}

void Display::begin() {
  _u8g2.begin();
  _u8g2.enableUTF8Print();
  _u8g2.setFont(u8g2_font_7x13_t_cyrillic);
}

void Display::showTemperature(float celsius) {
  _u8g2.clearBuffer();
  _u8g2.drawFrame(0, 0, WIDTH, HEIGHT);
  _u8g2.setCursor(3, 12);
  if (isnan(celsius)) {
    _u8g2.print("sensor error");
  } else {
    _u8g2.printf("%.1f C", celsius);
  }
  _u8g2.sendBuffer();
}
