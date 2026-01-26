#include "oled_ui.h"
#include "config.h"

namespace OLEDUI {

// Global OLED object
static Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static bool initialized = false;
static unsigned long lastUpdate = 0;

bool initialize(uint8_t address) {
  if (address == 0x00) {
    LOG_ERROR("Invalid I2C address for OLED");
    return false;
  }
  
  LOG_INFO("Initializing OLED at 0x%02X...", address);
  
  // Attempt initialization with detected address
  if (!display.begin(SSD1306_SWITCHCAPVCC, address)) {
    LOG_ERROR("OLED initialization failed at 0x%02X", address);
    return false;
  }
  
  initialized = true;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("mini-doom OLED OK!");
  display.display();
  
  LOG_INFO("OLED successfully initialized");
  return true;
}

void displayStatus(bool espOK, bool oledOK, uint8_t i2cAddr) {
  if (!initialized) {
    LOG_ERROR("OLED not initialized, cannot display status");
    return;
  }
  
  unsigned long now = millis();
  if (now - lastUpdate < 500) {
    return;  // Throttle updates
  }
  lastUpdate = now;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  // Title
  display.println("=== mini-doom ===");
  display.println();
  
  // Status indicators
  display.print("ESP32:  ");
  display.println(espOK ? "OK" : "FAIL");
  
  display.print("OLED:   ");
  display.println(oledOK ? "OK" : "FAIL");
  
  display.print("I2C:    0x");
  display.println(i2cAddr, HEX);
  
  display.println();
  display.print("Uptime: ");
  display.print(now / 1000);
  display.println("s");
  
  display.display();
}

void displayError(const char* errorMsg) {
  if (!initialized) {
    return;
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ERROR:");
  display.println(errorMsg);
  display.display();
}

bool isInitialized() {
  return initialized;
}

void runOLEDTestMode() {
  if (!initialized) {
    LOG_ERROR("OLED Test Mode: Display not initialized");
    return;
  }
  
  LOG_INFO("Running in OLED TEST MODE");
  
  while (true) {
    displayStatus(true, true, 0x3C);  // Demo status
    delay(1000);
  }
}

} // namespace OLEDUI
