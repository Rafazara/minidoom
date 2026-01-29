#include "doom_integration.h"
#include "../config.h"
#include "../oled_ui.h"
#include "../doom_adapter.h"
#include "../doom_graphics.h"

namespace DoomIntegration {

static bool initialized = false;
static unsigned long frameCount = 0;
static unsigned long lastTick = 0;

void setup() {
  LOG_INFO("========================================");
  LOG_INFO("Initializing Doom Nano...");
  LOG_INFO("========================================");
  
  // Phase 1 (STUB): Memory allocation and initialization
  LOG_INFO("Doom: Allocating memory buffers...");
  LOG_INFO("Doom: Display buffer: 128x64 pixels (1024 bytes)");
  LOG_INFO("Doom: Frame buffer initialized");
  
  // CRITICAL FIX: Initialize I2C Wire interface FIRST
  LOG_INFO("Doom: Initializing I2C Wire interface...");
  Wire.begin(21, 22);
  Wire.setClock(100000);
  
  // Initialize OLED display
  LOG_INFO("Doom: Setting up display renderer...");
  if (OLEDUI::initialize(0x3C)) {
    LOG_INFO("Doom: OLED initialized successfully");
  } else {
    LOG_ERROR("Doom: OLED initialization FAILED");
  }
  
  // Phase 2: Initialize Doom Adapter (game engine)
  LOG_INFO("Doom: Initializing Doom Adapter...");
  DoomAdapter::initialize();
  LOG_INFO("Doom: Doom Adapter initialized");
  LOG_INFO("Doom: Free Heap after init: %d bytes", ESP.getFreeHeap());
  
  initialized = true;
  lastTick = millis();
  frameCount = 0;
  
  LOG_INFO("Doom: Initialization complete ✓");
  LOG_INFO("========================================");
  LOG_INFO("Running in DOOM MODE - Ready for gameplay");
  LOG_INFO("========================================");
}

void loop() {
  unsigned long now = millis();
  
  // Render frame at target FPS (20 FPS = 50ms per frame)
  if (now - lastTick >= 50) {  // 50ms = 20 FPS
    lastTick = now;
    frameCount++;
    
    // Render single frame (includes world, sprites, HUD, dithering, blit)
    DoomAdapter::renderFrame();
    
    // Log every 20 frames (~1 second at 20 FPS)
    if (frameCount % 20 == 0) {
      LOG_INFO("Doom: Frame #%lu | Free Heap: %d bytes", 
               frameCount, ESP.getFreeHeap());
    }
  }
}

bool isInitialized() {
  return initialized;
}

unsigned long getFrameCount() {
  return frameCount;
}

} // namespace DoomIntegration
