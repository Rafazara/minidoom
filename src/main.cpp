// ============================================================
// MINI-DOOM MAIN ENTRY POINT
// Doom Nano integration with SSD1306 OLED display
// ============================================================

#include <Arduino.h>
#include "config.h"
#include "doom/doom_integration.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Initialize Doom engine
  DoomIntegration::setup();
}

void loop() {
  // Run Doom game loop
  DoomIntegration::loop();
}
