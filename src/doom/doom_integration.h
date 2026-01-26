#ifndef DOOM_INTEGRATION_H
#define DOOM_INTEGRATION_H

#include <Arduino.h>

/**
 * Doom Nano Integration Module (Stub)
 * 
 * This module handles the integration of Doom Nano engine with the mini-doom
 * firmware. Currently implemented as a stub/placeholder while the actual
 * rendering and game logic are being ported from the doom-nano submodule.
 * 
 * Future phases:
 * 1. Port doom-nano.ino game loop
 * 2. Implement rendering adapter for 128x64 OLED
 * 3. Integrate input handling (button mapping)
 * 4. Optimize memory and performance for ESP32
 */

namespace DoomIntegration {
  
  /**
   * Initialize Doom engine
   * - Allocate memory
   * - Setup display buffer
   * - Load game state
   */
  void setup();
  
  /**
   * Main game loop tick
   * - Process input
   * - Update game state
   * - Render frame
   */
  void loop();
  
  /**
   * Check if Doom is ready to run
   */
  bool isInitialized();
  
  /**
   * Get current frame count
   */
  unsigned long getFrameCount();

}

#endif // DOOM_INTEGRATION_H
