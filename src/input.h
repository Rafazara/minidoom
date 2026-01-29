#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/**
 * Input Handler Module - Phase 2.3
 * 
 * Purpose:
 * Isolated GPIO reading for game input (buttons)
 * Non-blocking debouncing with timestamp-based filtering
 * Compact state representation (bitmask)
 * 
 * Design:
 * - GPIO pins defined in config.h
 * - Software debounce (20ms threshold)
 * - Bitmask state (UP=0x01, DOWN=0x02, LEFT=0x04, RIGHT=0x08, FIRE=0x10)
 * - No dynamic allocation
 * - Decoupled from game engine
 * 
 * Integration:
 * Call readInput() before doomTick() each frame
 * Query state with getInput() or individual getInputXXX() functions
 */

namespace Input {

// ============================================================================
// INPUT BITMASK DEFINITIONS
// ============================================================================
#define INPUT_BTN_UP       0x01
#define INPUT_BTN_DOWN     0x02
#define INPUT_BTN_LEFT     0x04
#define INPUT_BTN_RIGHT    0x08
#define INPUT_BTN_FIRE     0x10

/**
 * Initialize input system
 * Sets GPIO pins to INPUT mode
 * Clears state buffer
 */
void initialize();

/**
 * Read GPIO states and apply debounce
 * Call every frame (before doomTick)
 * Non-blocking, <1ms execution
 */
void readInput();

/**
 * Get combined input state (bitmask)
 * @return: uint8_t with INPUT_* bits set
 */
uint8_t getInput();

/**
 * Individual input queries
 */
bool isInputUp();
bool isInputDown();
bool isInputLeft();
bool isInputRight();
bool isInputFire();

/**
 * Check if any movement input is active
 * @return: true if UP, DOWN, LEFT, or RIGHT is pressed
 */
bool isMoving();

/**
 * Get input debug string (for logging)
 * Format: "U D L R F"
 */
const char* getInputDebugString();

/**
 * Check initialization status
 */
bool isInitialized();

}  // namespace Input

#endif // INPUT_H
