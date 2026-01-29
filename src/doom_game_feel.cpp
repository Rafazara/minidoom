/**
 * Doom Game Feel System - Phase 3.4 Implementation
 *
 * Micro-animations and sensory feedback without performance cost.
 * All effects are 1-3 frames max, <0.5ms render overhead.
 */

#include "doom_game_feel.h"
#include <Arduino.h>
#include <string.h>

namespace DoomGameFeel {

// ============================================================================
// MODULE STATE
// ============================================================================

static bool g_initialized = false;

// Effect flags (set by trigger functions, consumed by render, cleared after)
static bool g_weapon_fire_active = false;
static bool g_enemy_hit_active = false;
static bool g_enemy_death_active = false;
static bool g_player_damage_active = false;
static bool g_low_health_active = false;
static bool g_wave_start_active = false;
static uint8_t g_wave_start_number = 0;
static bool g_wave_clear_active = false;
static uint8_t g_wave_clear_number = 0;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Set a pixel in the framebuffer (1-bit mode)
 */
static inline void setPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (x >= 128 || y >= 64) return;
    
    uint16_t byte_idx = (y / 8) * 128 + x;
    uint8_t bit_idx = y % 8;
    framebuffer[byte_idx] |= (1 << bit_idx);
}

/**
 * Clear a pixel in the framebuffer
 */
static inline void clearPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (x >= 128 || y >= 64) return;
    
    uint16_t byte_idx = (y / 8) * 128 + x;
    uint8_t bit_idx = y % 8;
    framebuffer[byte_idx] &= ~(1 << bit_idx);
}

/**
 * Draw weapon fire flash: radial pattern around crosshair (64, 32)
 * 1 frame effect, 4-8 pixels in diagonal cross pattern
 */
static void drawWeaponFireFlash(uint8_t* framebuffer) {
    uint8_t cx = 64;  // Crosshair center X
    uint8_t cy = 32;  // Crosshair center Y
    
    // Radial flash: 8 pixels in cardinal+diagonal directions, 2px away
    setPixel(framebuffer, cx - 2, cy - 2);  // Top-left diagonal
    setPixel(framebuffer, cx + 2, cy - 2);  // Top-right diagonal
    setPixel(framebuffer, cx - 2, cy + 2);  // Bottom-left diagonal
    setPixel(framebuffer, cx + 2, cy + 2);  // Bottom-right diagonal
    setPixel(framebuffer, cx - 3, cy);      // Left
    setPixel(framebuffer, cx + 3, cy);      // Right
    setPixel(framebuffer, cx, cy - 3);      // Top
    setPixel(framebuffer, cx, cy + 3);      // Bottom
}

/**
 * Draw enemy hit spark: 2-3 pixels at center of screen
 * Used to confirm hit without duplicating existing hit logic
 */
static void drawEnemyHitSpark(uint8_t* framebuffer) {
    // Small spark cluster: 3 pixels in a small triangle pattern, center screen
    uint8_t cx = 64;
    uint8_t cy = 32;
    
    setPixel(framebuffer, cx, cy);        // Center
    setPixel(framebuffer, cx - 1, cy + 1);  // Bottom-left
    setPixel(framebuffer, cx + 1, cy + 1);  // Bottom-right
}

/**
 * Draw enemy death crack: broken vertical pixels (1 frame)
 * Pattern: vertical line with gaps to create "broken" effect
 */
static void drawEnemyCrack(uint8_t* framebuffer) {
    // Vertical crack pattern: left-center area of screen
    uint8_t cx = 40;  // Off-center, near left side of enemies
    
    // Draw vertical line with every 3rd pixel missing (crack effect)
    setPixel(framebuffer, cx, 20);
    setPixel(framebuffer, cx, 21);
    // Gap
    setPixel(framebuffer, cx, 23);
    setPixel(framebuffer, cx, 24);
    // Gap
    setPixel(framebuffer, cx, 26);
    setPixel(framebuffer, cx, 27);
    
    // Offset crack (2 pixels right)
    setPixel(framebuffer, cx + 2, 19);
    setPixel(framebuffer, cx + 2, 20);
    // Gap
    setPixel(framebuffer, cx + 2, 22);
    setPixel(framebuffer, cx + 2, 23);
}

/**
 * Draw low health tick: small indicator at HUD corner
 * Synced with HUD blink: only visible when HUD is visible
 */
static void drawLowHealthTick(uint8_t* framebuffer) {
    // Small 1px tick at top-right corner of HUD (near AMMO display)
    // Position: x=106, y=49 (just outside HUD border)
    setPixel(framebuffer, 106, 49);
    setPixel(framebuffer, 107, 49);
}

/**
 * Draw wave start underline: brief underline under wave text area
 * Position: y=52 (in HUD area, might overlap with "WAVE N" text from GameFlow)
 */
static void drawWaveStartUnderline(uint8_t* framebuffer) {
    // Underline effect: 10 pixels wide, centered
    for (uint8_t i = 0; i < 10; i++) {
        setPixel(framebuffer, 59 + i, 52);  // Centered at x~64
    }
}

/**
 * Draw wave clear flourish: small border effect around "CLEAR" text
 * Position: y=52-54 (HUD area)
 */
static void drawWaveClearFlourish(uint8_t* framebuffer) {
    // Small rectangle flourish: 12 pixels wide, 2 pixels tall
    for (uint8_t i = 0; i < 12; i++) {
        setPixel(framebuffer, 58 + i, 51);  // Top line
        setPixel(framebuffer, 58 + i, 53);  // Bottom line
    }
}

/**
 * Optional: Emit sound via piezo/buzzer (if connected)
 * Currently: stub (no hardware configured)
 * Future: Integrate with available buzzer/speaker
 */
static void emitBeep(uint16_t frequency_hz, uint16_t duration_ms) {
    // TODO: Integrate with available buzzer hardware
    // For now: silent stub
    (void)frequency_hz;
    (void)duration_ms;
}

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

void initialize() {
    if (g_initialized) {
        return;
    }
    
    Serial.println(F("[GAME-FEEL] Initialized (Phase 3.4)"));
    g_initialized = true;
}

bool isInitialized() {
    return g_initialized;
}

void triggerWeaponFire() {
    if (!g_initialized) return;
    
    g_weapon_fire_active = true;
    emitBeep(1000, 50);  // Short beep: 1000 Hz, 50ms
}

void triggerEnemyHit() {
    if (!g_initialized) return;
    
    g_enemy_hit_active = true;
    emitBeep(800, 80);  // Medium beep: 800 Hz, 80ms
}

void triggerEnemyDeath() {
    if (!g_initialized) return;
    
    g_enemy_death_active = true;
    // Double-beep: two short beeps with slight gap
    emitBeep(600, 60);
    delay(20);
    emitBeep(600, 60);
}

void triggerPlayerDamage() {
    if (!g_initialized) return;
    
    g_player_damage_active = true;
    emitBeep(300, 100);  // Grave beep: 300 Hz, 100ms
}

void triggerLowHealthWarning(uint8_t player_health) {
    if (!g_initialized) return;
    
    // Only trigger if actually low
    if (player_health < 30) {
        g_low_health_active = true;
        // Light warning tone: 500 Hz, 40ms
        emitBeep(500, 40);
    }
}

void triggerWaveStart(uint8_t wave) {
    if (!g_initialized) return;
    
    g_wave_start_active = true;
    g_wave_start_number = wave;
    // Ascending beep: sweep from 600 to 900 Hz (simulated as single tone)
    emitBeep(750, 100);
}

void triggerWaveClear(uint8_t wave) {
    if (!g_initialized) return;
    
    g_wave_clear_active = true;
    g_wave_clear_number = wave;
    // Descending beep: sweep from 900 to 600 Hz (simulated as single tone)
    emitBeep(750, 100);
}

void render(uint8_t* framebuffer, uint32_t current_time_ms, uint8_t player_health) {
    if (!g_initialized || !framebuffer) return;
    
    // ========================================================================
    // COMBAT MICRO-ANIMATIONS (1 frame each)
    // ========================================================================
    
    // Weapon fire flash: radial pattern around crosshair
    if (g_weapon_fire_active) {
        drawWeaponFireFlash(framebuffer);
        g_weapon_fire_active = false;  // 1-frame effect
    }
    
    // Enemy hit spark: small 3-pixel cluster
    if (g_enemy_hit_active) {
        drawEnemyHitSpark(framebuffer);
        g_enemy_hit_active = false;  // 1-frame effect
    }
    
    // Enemy death crack: vertical broken line
    if (g_enemy_death_active) {
        drawEnemyCrack(framebuffer);
        g_enemy_death_active = false;  // 1-frame effect
    }
    
    // Player damage: already handled by DoomFeedback (vignette)
    // This just triggers the audio beep
    if (g_player_damage_active) {
        g_player_damage_active = false;  // Audio only, reset immediately
    }
    
    // ========================================================================
    // STATE INDICATORS (synchronized with game state)
    // ========================================================================
    
    // Low health tick: synchronized with HUD blink pattern
    if (player_health > 0 && player_health < 30) {
        // Blink interval: 500ms (synchronized with HUD)
        uint32_t blink_cycle = current_time_ms % 1000;
        if (blink_cycle < 500) {
            // "On" phase: show tick
            drawLowHealthTick(framebuffer);
        }
        // "Off" phase: tick disappears naturally
    }
    
    // ========================================================================
    // GAME FLOW MICRO-ANIMATIONS (1 frame each)
    // ========================================================================
    
    // Wave start underline
    if (g_wave_start_active) {
        drawWaveStartUnderline(framebuffer);
        g_wave_start_active = false;  // 1-frame effect
    }
    
    // Wave clear flourish
    if (g_wave_clear_active) {
        drawWaveClearFlourish(framebuffer);
        g_wave_clear_active = false;  // 1-frame effect
    }
    
    // Note: Effect flags are consumed (cleared) after rendering
    // Each trigger() call sets the flag, render() consumes it (1-frame effect)
    // Next frame: flag is false, effect is gone
}

}  // namespace DoomGameFeel
