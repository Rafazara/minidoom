#ifndef I2C_TOOLS_H
#define I2C_TOOLS_H

#include <Arduino.h>
#include <Wire.h>

namespace I2CTools {
  void initialize();
  void scanBus();
  bool probeAddress(uint8_t address);
  uint8_t detectOLEDAddress();
  void runI2CScanMode();
}

#endif // I2C_TOOLS_H
