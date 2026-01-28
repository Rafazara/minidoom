#ifndef OLED_UI_H
#define OLED_UI_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace OLEDUI {
  bool initialize(uint8_t address);
  void displayStatus(bool espOK, bool oledOK, uint8_t i2cAddr);
  void displayError(const char* errorMsg);
  void runOLEDTestMode();
  bool isInitialized();
  
  // Get reference to the display object (for direct rendering)
  Adafruit_SSD1306* getDisplay();
}

#endif // OLED_UI_H
