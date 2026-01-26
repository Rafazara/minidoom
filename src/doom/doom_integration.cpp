#include "doom_integration.h"
#include "../config.h"

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
  
  // Phase 1 (STUB): Placeholder for display setup
  LOG_INFO("Doom: Setting up display renderer...");
  LOG_INFO("Doom: Doom renderer ready (STUB - no rendering yet)");
  
  // Phase 1 (STUB): Placeholder for game state
  LOG_INFO("Doom: Initializing game state...");
  LOG_INFO("Doom: Game state ready");
  LOG_INFO("Doom: Free Heap after init: %d bytes", ESP.getFreeHeap());
  
  initialized = true;
  lastTick = millis();
  frameCount = 0;
  
  LOG_INFO("Doom: Initialization complete ✓");
  LOG_INFO("========================================");
  LOG_INFO("Running in DOOM MODE - Waiting for phase 2 integration");
  LOG_INFO("========================================");
}

void loop() {
  unsigned long now = millis();
  
  // Only tick every 1000ms for now (stub)
  if (now - lastTick >= 1000) {
    lastTick = now;
    frameCount++;
    
    LOG_INFO("Doom: Tick #%lu | Frame: %lu | Free Heap: %d bytes", 
             frameCount, frameCount, ESP.getFreeHeap());
    
    // STUB: In Phase 2, this will:
    // 1. Read input (buttons)
    // 2. Update game entities
    // 3. Render frame to OLED
    // 4. Update frame counter
  }
}

bool isInitialized() {
  return initialized;
}

unsigned long getFrameCount() {
  return frameCount;
}

} // namespace DoomIntegration
