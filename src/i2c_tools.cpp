#include "i2c_tools.h"
#include "config.h"

namespace I2CTools {

static uint8_t oledAddress = 0x00;

void initialize() {
  Wire.begin(OLED_SDA, OLED_SCL);
  LOG_INFO("I2C Bus Initialized (SDA=%d, SCL=%d)", OLED_SDA, OLED_SCL);
}

bool probeAddress(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

void scanBus() {
  LOG_INFO("Scanning I2C bus...");
  int deviceCount = 0;
  
  for (uint8_t addr = 0x01; addr < 0x7F; addr++) {
    if (probeAddress(addr)) {
      LOG_INFO("Device found at 0x%02X", addr);
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    LOG_ERROR("No I2C devices found!");
  } else {
    LOG_INFO("Total devices found: %d", deviceCount);
  }
}

uint8_t detectOLEDAddress() {
  LOG_INFO("Detecting OLED address...");
  
  if (probeAddress(OLED_ADDRESS_PRIMARY)) {
    oledAddress = OLED_ADDRESS_PRIMARY;
    LOG_INFO("OLED found at 0x%02X (primary)", OLED_ADDRESS_PRIMARY);
    return OLED_ADDRESS_PRIMARY;
  }
  
  if (probeAddress(OLED_ADDRESS_SECONDARY)) {
    oledAddress = OLED_ADDRESS_SECONDARY;
    LOG_INFO("OLED found at 0x%02X (secondary)", OLED_ADDRESS_SECONDARY);
    return OLED_ADDRESS_SECONDARY;
  }
  
  LOG_ERROR("OLED not detected at 0x%02X or 0x%02X", 
            OLED_ADDRESS_PRIMARY, OLED_ADDRESS_SECONDARY);
  oledAddress = 0x00;
  return 0x00;
}

uint8_t getDetectedAddress() {
  return oledAddress;
}

void runI2CScanMode() {
  LOG_INFO("Running in I2C SCAN MODE");
  LOG_INFO("Scanning I2C devices every %d ms...", I2C_SCAN_DELAY);
  
  while (true) {
    scanBus();
    delay(I2C_SCAN_DELAY);
  }
}

} // namespace I2CTools
