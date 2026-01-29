#ifndef DOOM_GAME_OVER_H
#define DOOM_GAME_OVER_H

#include <stdint.h>

/**
 * Doom Game Over Screen - Phase 2.13
 *
 * Dedicated GAME OVER state screen for mini-doom:
 * - Static background (black)
 * - "YOU DIED" centered near top
 * - "PRESS FIRE" blinking near bottom
 *
 * No interaction with world, sprites, HUD or input module.
 * All rendering is done directly into the shared 1-bit framebuffer.
 */

namespace DoomGameOver {

/**
 * Initialize game over screen subsystem.
 * Logs: [GAME_OVER] Initialized
 */
void initialize();

/**
 * Render game over screen into framebuffer.
 *
 * @param framebuffer Pointer to 128x64 / 8 = 1024-byte framebuffer
 * @param current_time_ms Current system time (for blinking)
 *
 * Behavior:
 * - Clears framebuffer to black
 * - Draws "YOU DIED" centered at y≈18
 * - Draws blinking "PRESS FIRE" centered at y≈40
 *   (blinks every 500ms by skipping draw when off)
 * - Logs periodic rendering messages without spamming
 */
void render(uint8_t* framebuffer, uint32_t current_time_ms);

/**
 * Decide whether game should restart from game over screen.
 *
 * @param fire_pressed Edge-detected FIRE state (true only when just pressed)
 * @return true if game should transition GAME_OVER → PLAYING
 *
 * Note: This function is pure and does not keep internal input state.
 * Edge detection must be handled by the caller (e.g. DoomAdapter).
 */
bool shouldRestartGame(bool fire_pressed);

}  // namespace DoomGameOver

#endif  // DOOM_GAME_OVER_H

