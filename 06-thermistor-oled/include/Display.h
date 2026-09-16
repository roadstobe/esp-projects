#pragma once
#include <U8g2lib.h>

class Display {
public:
  Display(uint8_t sdaPin, uint8_t sclPin);

  void begin();
  void showTemperature(float celsius);

private:
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C _u8g2;
};
