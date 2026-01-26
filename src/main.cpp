#include <Arduino.h>
#include "config.h"
#include "diagnostics.h"
#include "i2c_tools.h"
#include "oled_ui.h"

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
void runMode();

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(500);
  
  LOG_INFO("========================================");
  LOG_INFO("mini-doom - Doom Nano on ESP32");
  LOG_INFO("========================================");
  
  // Print active mode
  #ifdef MODE_DIAG
    LOG_INFO("Active Mode: DIAGNOSTIC");
  #elif defined MODE_I2C_SCAN
    LOG_INFO("Active Mode: I2C_SCAN");
  #elif defined MODE_OLED_TEST
    LOG_INFO("Active Mode: OLED_TEST");
  #endif
  
  LOG_INFO("========================================");
  
  // Initialize modules
  Diagnostics::initialize();
  I2CTools::initialize();
  
  // Select and run mode
  runMode();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  // Main loop - mode-specific behavior
  
  #ifdef MODE_DIAG
    Diagnostics::printHeartbeat();
    delay(100);
  #endif
  
  #ifdef MODE_I2C_SCAN
    // I2CScan mode runs in infinite loop in runMode()
  #endif
  
  #ifdef MODE_OLED_TEST
    // OLED Test mode runs in infinite loop in runMode()
  #endif
}

// ============================================================================
// MODE SELECTION & EXECUTION
// ============================================================================
void runMode() {
  #ifdef MODE_DIAG
    Diagnostics::runDiagnosticMode();
    
  #elif defined MODE_I2C_SCAN
    I2CTools::runI2CScanMode();
    
  #elif defined MODE_OLED_TEST
    LOG_INFO("Initializing OLED...");
    uint8_t oledAddr = I2CTools::detectOLEDAddress();
    
    if (oledAddr == 0x00) {
      LOG_ERROR("OLED not detected! Falling back to diagnostic mode.");
      Diagnostics::printSystemInfo();
      while (true) {
        Diagnostics::printHeartbeat();
        delay(100);
      }
    }
    
    if (OLEDUI::initialize(oledAddr)) {
      LOG_INFO("OLED initialized successfully");
      OLEDUI::displayStatus(true, true, oledAddr);
      LOG_INFO("OLED display ready");
    } else {
      LOG_ERROR("Failed to initialize OLED");
      while (true) {
        Diagnostics::printHeartbeat();
        delay(100);
      }
    }
  #else
    // Default to diagnostics if no mode selected
    Diagnostics::runDiagnosticMode();
  #endif
}