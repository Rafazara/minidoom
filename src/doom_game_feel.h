#ifndef DOOM_GAME_FEEL_H
#define DOOM_GAME_FEEL_H

#include <stdint.h>

/**
 * Doom Game Feel System - Phase 3.4
 *
 * Micro-animations and sensory feedback that reinforce player actions without impact on performance.
 * 
 * Features:
 * - Weapon fire: Small radial flash around crosshair (1 frame)
 * - Enemy hit: Spark effect (2-3 pixels, 1 frame)
 * - Enemy death: Crack visual (1 frame)
 * - Low health: Subtle tick indicator (synchronized with HUD)
 * - Wave start/clear: Visual flourish (1 frame)
 * - Sound effects: Optional beeps via piezo/buzzer (if available)
 *
 * Philosophy:
 * Not about "seeing more things" - about feeling better what already exists.
 * Good game feedback doesn't shout; it responds.
 *
 * Performance: <0.5ms per frame, all effects 1-3 frames max
 *
 * No state exposure, no circular dependencies.
 */

namespace DoomGameFeel {

/**
 * Initialize game feel system.
 * Logs: [GAME-FEEL] Initialized (Phase 3.4)
 */
void initialize();

// ============================================================================
// COMBAT FEEDBACK TRIGGERS
// ============================================================================

/**
 * Trigger weapon fire micro-feedback.
 * Visual: Small radial flash around crosshair (1 frame, 4-8 pixels in diagonal cross)
 * Audio: Short beep (if piezo available)
 * 
 * Called from: DoomSpriteRenderer::handleCombat()
 */
void triggerWeaponFire();

/**
 * Trigger enemy hit micro-feedback.
 * Visual: Small spark at impact (2-3 pixels, 1 frame)
 * Audio: Medium beep (if piezo available)
 * 
 * Called from: DoomSpriteRenderer::handleCombat()
 */
void triggerEnemyHit();

/**
 * Trigger enemy death micro-feedback.
 * Visual: Crack effect (broken vertical pixels, 1 frame)
 * Audio: Double-beep (if piezo available)
 * 
 * Called from: DoomSpriteRenderer::handleCombat()
 */
void triggerEnemyDeath();

// ============================================================================
// PLAYER FEEDBACK TRIGGERS
// ============================================================================

/**
 * Trigger player damage visual feedback.
 * Visual: Already handled by DoomFeedback system (damage direction vignette)
 * Audio: Grave beep (if piezo available)
 * 
 * Called from: DoomAdapter::applyPlayerDamageFX()
 */
void triggerPlayerDamage();

/**
 * Trigger low health indicator (when health < 30%).
 * Visual: Subtle tick in HUD corner (1px, synchronized with HUD blink)
 * Audio: Light pulse/warning tone (if piezo available)
 * 
 * Called from: render() based on player_health
 */
void triggerLowHealthWarning(uint8_t player_health);

// ============================================================================
// GAME FLOW FEEDBACK TRIGGERS
// ============================================================================

/**
 * Trigger wave start feedback.
 * Visual: Brief underline effect on wave text (1 frame)
 * Audio: Ascending beep tone (if piezo available)
 * 
 * Called from: DoomGameFlow::update() or integration point
 */
void triggerWaveStart(uint8_t wave);

/**
 * Trigger wave clear feedback.
 * Visual: Brief flourish around "CLEAR" text (1 frame)
 * Audio: Descending beep tone (if piezo available)
 * 
 * Called from: DoomGameFlow::update() or integration point
 */
void triggerWaveClear(uint8_t wave);

// ============================================================================
// RENDERING
// ============================================================================

/**
 * Render all micro-animation overlays and sound feedback.
 * Called once per frame in the pipeline:
 *   renderWorld → renderSprites → renderHUD → renderFeedbackFX → 
 *   renderGameFlow → renderGameFeel ← THIS FUNCTION
 *
 * @param framebuffer: 128×64 / 8 = 1024-byte framebuffer (already has world, sprites, HUD)
 * @param current_time_ms: Current system time for effect synchronization
 * @param player_health: Current player health (0-100) for low health indicator
 *
 * All effects are 1-3 frames max. After rendering, internal state resets.
 * 
 * Performance: <0.5ms per frame (no loops >10 pixels)
 */
void render(uint8_t* framebuffer, uint32_t current_time_ms, uint8_t player_health);

// ============================================================================
// FEATURE FLAGS & DIAGNOSTICS
// ============================================================================

/**
 * Check if game feel system initialized.
 */
bool isInitialized();

}  // namespace DoomGameFeel

#endif  // DOOM_GAME_FEEL_H
