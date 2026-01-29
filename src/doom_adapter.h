#ifndef DOOM_ADAPTER_H
#define DOOM_ADAPTER_H

#include <stdint.h>
#include <string.h>
#include "doom_game_flow.h"
#include "doom_game_feel.h"

/**
 * Doom Nano Adapter - Phase 2.7 Combat Feedback Effects
 * 
 * Purpose: 
 * Connects Doom Nano raycasting engine to OLED display (128×64)
 * Manages player input, 3D camera updates, sprite rendering, combat detection, and visual feedback
 * Delivers perspective-correct walls, sprites with occlusion, hit detection, and combat FX
 * 
 * Architecture (Phase 2.7):
 * Input::readInput() → DoomAdapter::advanceGameTick() → DoomSpriteRenderer::handleCombat()
 *   → DoomAdapter::applyCombatFX() → DoomWorldRenderer::renderWorld() 
 *   → DoomSpriteRenderer::renderSprites() → DoomAdapter::renderFrame() → DoomGraphics::blit()
 * 
 * Combat Features:
 * - Central ray hit detection (screen_x = 64)
 * - ±2 column hit threshold (62-66 columns)
 * - Distance comparison vs depth buffer
 * - Enemy state tracking (ALIVE/DEAD)
 * - Edge-triggered fire (no continuous fire)
 * - Logging: [COMBAT] prefix for diagnostics
 * 
 * Visual Feedback (Phase 2.7):
 * - Weapon flash (1 frame, full white)
 * - Screen shake (±1 pixel, 1 frame)
 * - Logging: [COMBAT-FX] prefix for feedback
 * 
 * Performance Constraints:
 * - 20 FPS target (50ms frame budget)
 * - <15ms raycasting per frame
 * - <5ms sprite rendering per frame
 * - <2ms combat detection per frame
 * - <1ms combat FX per frame
 * - All static allocation (no malloc/new)
 * - Bayer 2×2 dithering for 1-bit monochrome conversion
 * 
 * Resolution:
 * - OLED: 128×64 pixels
 * - Rendering: 128 rays (one per column) × 64 pixels height
 * - Raycasting: DDA algorithm with distance-based shading (5 levels)
 * - Sprites: Max 4 simultaneous sprites with depth occlusion
 */

namespace DoomAdapter {

/**
 * Initialize Doom Nano rendering system
 * Sets up display, player position, initial scene
 * Logs startup progress with [DOOM-CORE] prefix
 */
void initialize();

/**
 * Render single frame from Doom Nano
 * Executes full pipeline: readInput → advanceGameTick → renderFrame → dither
 * Non-blocking with frame throttling (50ms = 20 FPS)
 * 
 * @return true if frame was rendered, false if throttled
 */
bool renderFrame();

/**
 * Advance game state by one tick
 * Updates player position/orientation based on current input
 * Called automatically by renderFrame()
 * 
 * Implementation:
 * - Reads Input::getInput() bitmask
 * - Updates g_player position based on UP/DOWN/LEFT/RIGHT
 * - Updates g_player rotation based on input
 * - Non-blocking, ~5ms per tick
 */
void advanceGameTick();

/**
 * Apply visual feedback effects for combat actions (Phase 2.7)
 * Called after handleCombat() to apply 1-frame visual effects
 * 
 * Effects Applied:
 * - Weapon flash: Full white weapon sprite for current frame
 * - Screen shake: ±1 pixel displacement in camera view
 * 
 * Implementation:
 * - Sets weaponFlash flag if fire was just pressed
 * - Sets screenShakeOffset if hit was detected
 * - Automatically resets flags after frame render
 * - Logs [COMBAT-FX] events for diagnostics
 * 
 * Performance: <1ms per frame
 */
void applyCombatFX();

/**
 * Apply visual feedback effects for player damage (Phase 2.10)
 * Called after applyCombatFX() to apply damage-specific effects
 * 
 * Effects Applied:
 * - Player damage flash: Screen flashes with invert pattern (different from weapon flash)
 * - Player damage shake: ±2 pixel displacement (different from combat shake)
 * 
 * Implementation:
 * - Sets player_damage_flash flag if player took damage
 * - Sets player_damage_shake offset if damage applied
 * - Automatically resets after frame render
 * - Logs [PLAYER-FX] events for diagnostics
 * 
 * Performance: <1ms per frame
 */
void applyPlayerDamageFX();

/**
 * Apply damage to player (Phase 2.10)
 * Reduces player health by specified amount
 * Triggers damage feedback effects and logging
 * 
 * @param damage: HP amount to subtract (typically ENEMY_ATTACK_DAMAGE)
 * 
 * Implementation:
 * - Subtracts damage from g_player.health (0-100)
 * - Clamps result to 0 (no negative health)
 * - Sets player_damage_flash and player_damage_shake flags
 * - Logs [PLAYER] Took damage event
 * - Logs [PLAYER] Health: X event
 * 
 * Performance: <1ms
 */
void applyPlayerDamage(uint8_t damage);

/**
 * Check if player is alive (Phase 2.10)
 * @return true if health > 0
 */
bool isPlayerAlive();

/**
 * Get screen shake offset for current frame (Phase 2.7)
 * Applied to world raycasting before rendering
 * 
 * @param out_x: Reference to receive X offset (±1 pixel)
 * @param out_y: Reference to receive Y offset (±1 pixel)
 */
void getScreenShakeOffset(int8_t& out_x, int8_t& out_y);

/**
 * Get screen shake offset for player damage (Phase 2.10)
 * Applied to world raycasting for damage effects
 * 
 * @param out_x: Reference to receive X offset (±2 pixel from damage)
 * @param out_y: Reference to receive Y offset (±2 pixel from damage)
 */
void getPlayerDamageShakeOffset(int8_t& out_x, int8_t& out_y);

/**
 * Check if weapon should be flashed this frame (Phase 2.7)
 * Used by sprite renderer to apply flash effect
 * 
 * @return true if weapon_flash flag is set
 */
bool getWeaponFlash();

/**
 * Check if player damage flash should be applied (Phase 2.10)
 * Used by renderer to apply inverted damage effect
 * 
 * @return true if player_damage_flash flag is set
 */
bool getPlayerDamageFlash();

/**
 * Get current Doom Nano game state (for diagnostics)
 * @return Player health (0-100)
 */
uint8_t getPlayerHealth();

/**
 * Get current ammunition count (Phase 2.11)
 * @return Current ammo (0-255)
 */
uint8_t getPlayerAmmo();

/**
 * Render HUD to framebuffer (Phase 2.11)
 * Called after renderSprites() in game loop
 * Displays health, ammo, and crosshair
 * 
 * @param framebuffer: Framebuffer pointer (128×64/8 = 1024 bytes)
 */
void renderHUDToFramebuffer(uint8_t* framebuffer);

/**
 * Check if Doom Nano is properly initialized
 * @return true if ready to render, false if failed
 */
bool isInitialized();

/**
 * Reset full game state (Phase 2.13)
 * Restores player, ammo, enemies and timers to initial values
 * Does NOT reset hardware or framebuffer directly
 * Logs: [GAME] Game state reset
 */
void resetGameState();

/**
 * Register world-space damage source for player (Phase 2.14)
 * Called by enemy AI just before applying damage,
 * used by feedback system to compute damage direction.
 */
void registerDamageSource(double enemy_x, double enemy_y);

}  // namespace DoomAdapter

#endif
