#ifndef DOOM_TITLE_H
#define DOOM_TITLE_H

#include <stdint.h>

/**
 * Doom Title Screen - Phase 2.12
 * 
 * Minimal title screen for mini-doom:
 * - Static background
 * - "MINI DOOM" centered near top
 * - "PRESS FIRE" blinking near bottom
 * 
 * No interaction with world, sprites, HUD or input module.
 * All rendering is done directly into the shared 1-bit framebuffer.
 */

namespace DoomTitle {

/**
 * Initialize title screen subsystem.
 * Logs: [TITLE] Initialized
 */
void initialize();

/**
 * Render title screen into framebuffer.
 * 
 * @param framebuffer Pointer to 128x64 / 8 = 1024-byte framebuffer
 * @param current_time_ms Current system time (for blinking)
 * 
 * Behavior:
 * - Clears framebuffer to black
 * - Draws "MINI DOOM" centered at y≈18
 * - Draws blinking "PRESS FIRE" centered at y≈40
 *   (blinks every 500ms by skipping draw when off)
 * - Logs periodic rendering messages without spamming
 */
void render(uint8_t* framebuffer, uint32_t current_time_ms);

/**
 * Decide whether game should start from title screen.
 * 
 * @param fire_pressed Edge-detected FIRE state (true only when just pressed)
 * @return true if game should transition TITLE → PLAYING
 * 
 * Note: This function is pure and does not keep internal input state.
 * Edge detection must be handled by the caller (e.g. DoomAdapter).
 */
bool shouldStartGame(bool fire_pressed);

}  // namespace DoomTitle

#endif  // DOOM_TITLE_H
