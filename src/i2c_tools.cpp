#include "i2c_tools.h"
#include "config.h"

namespace I2CTools {

static uint8_t oledAddress = 0x00;

void initialize() {
  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);  // Phase 4.0 RC: Set safe I2C speed for SSD1306
  
  // Phase 4.0 RC: Power-on stabilization delay
  // SSD1306 needs time to become responsive after power-up
  delay(100);
  
  LOG_INFO("I2C Bus Initialized (SDA=%d, SCL=%d, Clock=100kHz)", OLED_SDA, OLED_SCL);
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
  LOG_INFO("[I2C] OLED Address Detection started...");
  LOG_INFO("[I2C] Checking primary address (0x%02X)...", OLED_ADDRESS_PRIMARY);
  
  if (probeAddress(OLED_ADDRESS_PRIMARY)) {
    oledAddress = OLED_ADDRESS_PRIMARY;
    LOG_INFO("[I2C] ✓ OLED FOUND at 0x%02X (primary address)", OLED_ADDRESS_PRIMARY);
    return OLED_ADDRESS_PRIMARY;
  }
  
  LOG_INFO("[I2C] Not found at 0x%02X, checking secondary (0x%02X)...", 
           OLED_ADDRESS_PRIMARY, OLED_ADDRESS_SECONDARY);
  
  if (probeAddress(OLED_ADDRESS_SECONDARY)) {
    oledAddress = OLED_ADDRESS_SECONDARY;
    LOG_INFO("[I2C] ✓ OLED FOUND at 0x%02X (secondary address)", OLED_ADDRESS_SECONDARY);
    return OLED_ADDRESS_SECONDARY;
  }
  
  LOG_ERROR("[I2C] ✗ OLED NOT DETECTED");
  LOG_ERROR("[I2C] Checked: 0x%02X (primary) and 0x%02X (secondary)", 
            OLED_ADDRESS_PRIMARY, OLED_ADDRESS_SECONDARY);
  LOG_ERROR("[I2C] Troubleshooting:");
  LOG_ERROR("[I2C]   1. Verify SDA=GPIO21, SCL=GPIO22");
  LOG_ERROR("[I2C]   2. Check OLED power (3.3V VCC, GND)");
  LOG_ERROR("[I2C]   3. Try MODE_I2C_SCAN to find devices on bus");
  
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
