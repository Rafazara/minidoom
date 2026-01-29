#ifndef DOOM_FEEDBACK_H
#define DOOM_FEEDBACK_H

#include <stdint.h>

/**
 * Doom Feedback System - Phase 2.14
 *
 * Lightweight, 1-frame visual feedback for:
 * - Player damage direction (N/S/E/W style edge indicators)
 * - Damage vignette on screen corners
 * - Hit confirm flash at crosshair
 *
 * No interaction with world, sprites, HUD or input modules.
 * All rendering is done as a final overlay into the shared 1-bit framebuffer.
 */

namespace DoomFeedback {

/**
 * Initialize feedback subsystem.
 * Logs: [FEEDBACK] Initialized
 */
void initialize();

/**
 * Trigger feedback for player damage, including direction and vignette.
 *
 * @param player_x Player world X position
 * @param player_y Player world Y position
 * @param enemy_x  Enemy world X position (damage source)
 * @param enemy_y  Enemy world Y position (damage source)
 *
 * Logs: [PLAYER-FX] Damage direction: <dir>
 */
void triggerPlayerDamage(
    double player_x,
    double player_y,
    double enemy_x,
    double enemy_y
);

/**
 * Trigger hit-confirm feedback when enemy is hit.
 * Logs: [COMBAT-FX] Hit confirm
 */
void triggerHitConfirm();

/**
 * Render all pending 1-frame feedback overlays.
 * Called once per frame after HUD rendering, before blit.
 *
 * @param framebuffer Pointer to 128x64 / 8 = 1024-byte framebuffer
 */
void render(uint8_t* framebuffer);

}  // namespace DoomFeedback

#endif  // DOOM_FEEDBACK_H

