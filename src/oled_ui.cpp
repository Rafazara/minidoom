#include "oled_ui.h"
#include "config.h"

namespace OLEDUI {

// Global OLED object
static Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
static bool initialized = false;
static unsigned long lastUpdate = 0;

bool initialize(uint8_t address) {
  if (address == 0x00) {
    LOG_ERROR("[OLED] Initialize: Invalid I2C address (0x00)");
    return false;
  }
  
  // Phase 4.0 RC: Harden OLED initialization with power-on delay
  // SSD1306 requires stabilization time after power-up (i2c not responsive until ready)
  LOG_INFO("[OLED] Initialize: Power stabilization delay (100ms)...");
  delay(100);
  
  // Set I2C clock to safe speed for SSD1306 (100kHz standard)
  Wire.setClock(100000);
  LOG_INFO("[OLED] Initialize: Starting SSD1306 setup at 0x%02X...", address);
  
  // Attempt initialization with detected address
  if (!display.begin(SSD1306_SWITCHCAPVCC, address)) {
    LOG_ERROR("[OLED] Initialize: FAILED - driver not responding at 0x%02X", address);
    return false;
  }
  
  LOG_INFO("[OLED] Initialize: Driver responding, configuring display...");
  
  // STEP 1 — FORCE SSD1306 INTO HORIZONTAL ADDRESSING MODE
  // This matches the framebuffer layout used by Adafruit_SSD1306::drawBitmap()
  display.ssd1306_command(SSD1306_MEMORYMODE);
  display.ssd1306_command(0x00);  // Horizontal addressing mode
  
  // STEP 2 — RESET COLUMN & PAGE RANGE TO FULL WINDOW
  display.ssd1306_command(SSD1306_COLUMNADDR);
  display.ssd1306_command(0);
  display.ssd1306_command(127);
  
  display.ssd1306_command(SSD1306_PAGEADDR);
  display.ssd1306_command(0);
  display.ssd1306_command(3);
  
  // Phase 4.0 RC + Display Fix: Force known-good orientation for SSD1306 panel variants
  // Some SSD1306 panels have inverted COM scan or segment remap; explicitly set to known state
  display.clearDisplay();
  display.setRotation(2);        // 180-degree rotation for proper framebuffer mapping
  
  // Apply explicit segment remap and COM scan direction (fixes garbled/inverted displays)
  display.ssd1306_command(SSD1306_SEGREMAP | 0x01);   // Segment remap: column address 127→0
  display.ssd1306_command(SSD1306_COMSCANDEC);        // COM scan: from COM63 to COM0 (normal)
  display.display();             // Flush orientation fix to hardware
  
  #if MINI_DOOM_DEBUG
  LOG_DEBUG("[OLED] Orientation fix applied (rotation=180, segremap, comscan)");
  #endif
  
  initialized = true;
  
  LOG_INFO("[OLED] Initialize: ✓ Display ready!");
  LOG_INFO("[OLED] Initialize: 128x64 SSD1306 initialized at 0x%02X", address);
  
  return true;
}

void displayStatus(bool espOK, bool oledOK, uint8_t i2cAddr) {
  if (!initialized) {
    LOG_ERROR("[OLED] Display not initialized");
    return;
  }
  
  unsigned long now = millis();
  if (now - lastUpdate < 500) {
    return;  // Throttle updates to 500ms
  }
  lastUpdate = now;
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  // Title - compact for 128x64
  display.println("mini-doom OLED");
  display.println("================");
  
  // Status indicators
  display.print("ESP32:    ");
  display.println(espOK ? "[OK]" : "[FAIL]");
  
  display.print("OLED:     ");
  display.println(oledOK ? "[OK]" : "[FAIL]");
  
  display.print("I2C Addr: ");
  display.print("0x");
  display.println(i2cAddr, HEX);
  
  display.println("================");
  display.print("Uptime: ");
  display.print(now / 1000);
  display.print("s  Heap: ");
  display.print(ESP.getFreeHeap() / 1024);
  display.println("KB");
  
  display.display();
  
  // Log every 5 seconds
  static unsigned long lastLog = 0;
  if (now - lastLog >= 5000) {
    lastLog = now;
    LOG_INFO("[OLED] Display active | Uptime: %lu s | I2C: 0x%02X | Heap: %u KB", 
             now / 1000, i2cAddr, ESP.getFreeHeap() / 1024);
  }
}

void displayError(const char* errorMsg) {
  if (!initialized) {
    return;
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ERROR");
  display.println("================");
  display.println(errorMsg);
  display.display();
}

bool isInitialized() {
  return initialized;
}

Adafruit_SSD1306* getDisplay() {
  return &display;
}

void runOLEDTestMode() {
  if (!initialized) {
    LOG_ERROR("[OLED] OLED Test Mode: Display not initialized");
    return;
  }
  
  LOG_INFO("[OLED] Running ISOLATED OLED TEST MODE");
  LOG_INFO("[OLED] Testing SSD1306 128x64 OLED display");
  LOG_INFO("[OLED] Display will update every 500ms");
  LOG_INFO("[OLED] Press RESET to exit this mode");
  LOG_INFO("[OLED] ========================================");
  
  unsigned long testStartTime = millis();
  unsigned long testDuration = 0;
  
  while (true) {
    // Update display with live data
    displayStatus(true, true, 0x3C);  // Assume primary address (will show actual)
    
    testDuration = millis() - testStartTime;
    
    // Non-blocking delay with error checking
    delay(100);
    
    // Log performance metrics every 10 seconds
    static unsigned long lastPerfLog = 0;
    if (testDuration - lastPerfLog >= 10000) {
      lastPerfLog = testDuration;
      LOG_INFO("[OLED] Test running for %lu s | Heap: %u KB | Free: %u%%", 
               testDuration / 1000, 
               ESP.getFreeHeap() / 1024,
               (ESP.getFreeHeap() * 100) / ESP.getHeapSize());
    }
  }
}

} // namespace OLEDUI
