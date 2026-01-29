#ifndef DOOM_HUD_H
#define DOOM_HUD_H

#include <stdint.h>

/**
 * Doom HUD System - Phase 3.3
 * 
 * Polish & Visual Hierarchy - Doom-style HUD with clear visual precedence
 * Displays health, ammo, and crosshair with improved legibility and visual hierarchy
 * 
 * Screen Layout:
 * - World area: 128×48 pixels (y=0..47)
 * - HUD separator: 1px line at y=47
 * - HUD area: 128×16 pixels (y=48..63)
 * 
 * HUD Elements:
 * - Health display with thick bar and large number (left side, instantly readable)
 * - Ammo counter with clear "NO AMMO" state (right side, impossible to misread)
 * - Crosshair (center, 5×5 hollow for better visibility)
 * - Visual separator (horizontal line creating clear "foundation")
 * 
 * Features:
 * - Improved HP bar: 8×3 → 10×4 pixels, always bordered
 * - Large health number below bar for instant reading
 * - Ammo label + large number, special case "NO AMMO" when empty
 * - Refined crosshair: 5×5 hollow with empty center (better visibility on textures)
 * - Low health warning (blinks HP bar and number)
 * - No ammo warning (blinks "NO AMMO" text)
 * - Horizontal separator line (visual foundation, Doom aesthetic)
 * - Zero animations, static rendering
 * - No screen shake effect
 * - No dithering
 * 
 * Visual Philosophy:
 * Phase 3.2 made you recognize the world
 * Phase 3.3 makes you play without thinking - HUD disappears while informing
 * 
 * Performance: <1ms per frame (only small loops, no depth buffer reads)
 */

namespace DoomHUD {

// ============================================================================
// INITIALIZATION
// ============================================================================

/**
 * Initialize HUD system
 * Sets up fonts, constants, state
 * Called once during startup
 * Logs: [HUD] Initialized
 */
void initialize();

/**
 * Check if HUD is initialized
 * @return true if ready to render
 */
bool isInitialized();

// ============================================================================
// MAIN RENDERING
// ============================================================================

/**
 * Render complete HUD to framebuffer
 * Called once per frame after renderSprites()
 * 
 * @param framebuffer: Framebuffer pointer (128×64 / 8 = 1024 bytes)
 * @param player_health: Current player health (0-100)
 * @param player_ammo: Current ammunition count (0-255)
 * @param current_time_ms: Current system time for blinking effects
 * 
 * Renders in order:
 * 1. Background (horizontal line separator at y=47)
 * 2. Health display (left side)
 * 3. Ammo display (right side)
 * 4. Crosshair (center)
 * 
 * Effects:
 * - Low health blinking: when health < 30% blinks at 500ms interval
 * - No ammo blinking: when ammo == 0 blinks at 500ms interval
 * - Logging: [HUD] HP=X, Ammo=Y, Low health warning
 * 
 * Performance: <1ms per frame
 */
void renderHUD(
    uint8_t* framebuffer,
    uint8_t player_health,
    uint8_t player_ammo,
    uint32_t current_time_ms
);

// ============================================================================
// HUD ELEMENT RENDERING (Internal, called by renderHUD)
// ============================================================================

/**
 * Draw health display (left side of HUD)
 * 
 * Position: x=2, y=50
 * Format: "HP" small label + thick bar + large numeric value below
 * Range: 0-100 HP
 * 
 * Visual Hierarchy (left to right):
 *   [HP]  <- small label (3×5)
 *   ████░  <- thick bar (10×4, bordered)
 *   [ 56]  <- large number below (4×5)
 * 
 * Bar Details:
 *   - Size: 10 pixels wide × 4 pixels tall (improved from 8×3)
 *   - Always bordered (left and right edges visible)
 *   - Filled segments are solid
 *   - Empty segments show outline only
 *   - 10 segments total for finer granularity
 * 
 * Number Details:
 *   - Large 4×5 pixel characters
 *   - Right-aligned, directly below bar
 *   - Always visible (never blinks)
 *   - Blinks only when health < 30% (low health warning)
 * 
 * Blinking Behavior:
 *   - When health < 30%: Entire HP display (bar + number) blinks at 500ms interval
 *   - Creates urgency without loss of information
 *   - Readability always maintained
 * 
 * @param framebuffer: Framebuffer pointer
 * @param health: Player health value (0-100)
 * @param blink: true if currently in "off" phase of blink cycle (skip rendering)
 */
void drawHealth(uint8_t* framebuffer, uint8_t health, bool blink);

/**
 * Draw ammo display (right side of HUD)
 * 
 * Position: x=95, y=50 (right-aligned)
 * Format: "AMMO" small label + large numeric value OR "NO AMMO" blinking
 * 
 * Visual Hierarchy (right-aligned):
 *   [AMMO]  <- small label (3×5)
 *   [ 07]   <- large number (4×5)
 * 
 * Special State (ammo == 0):
 *   [NO AMMO]  <- clear warning text
 *   Blinks at 500ms interval (on/off visible clearly)
 *   Impossible to miss - occupies same position as number
 * 
 * Number Details:
 *   - Large 4×5 pixel characters
 *   - Right-aligned on screen
 *   - Always visible when ammo > 0
 *   - Directly below label
 * 
 * Label Details:
 *   - "AMMO" text (3×5 per character)
 *   - Always visible (provides context)
 *   - Help player understand what the number means
 * 
 * @param framebuffer: Framebuffer pointer
 * @param ammo: Current ammo count (0-255, typically 0-99)
 * @param blink: true if currently in "off" phase of blink cycle (skip rendering on ammo == 0)
 */
void drawAmmo(uint8_t* framebuffer, uint8_t ammo, bool blink);

/**
 * Draw crosshair (center of screen)
 * 
 * Position: x=64, y=32 (center of world view)
 * Style: 5×5 hollow cross with empty center (improved from 3×3 solid)
 * 
 * Visual Design:
 * ```
 *     #
 *     #
 *   # # #
 *     #
 *     #
 * ```
 * 
 * Improvements from Phase 2.11:
 *   - Size: 3×3 → 5×5 pixels (larger, more visible)
 *   - Style: solid → hollow (center empty, better visibility on textured walls)
 *   - Tip arms: extended 2px in each direction
 *   - Center: deliberately empty for sight-line clarity
 * 
 * Benefits:
 *   - More visible against wall textures and sprites
 *   - Center clear allows seeing what you're aiming at
 *   - Classic Doom aesthetic (empty center)
 *   - No confusion with enemy sprites
 * 
 * @param framebuffer: Framebuffer pointer
 */
void drawCrosshair(uint8_t* framebuffer);

// ============================================================================
// HUD CONSTANTS - Phase 3.3
// ============================================================================

// Screen dimensions
#define HUD_SCREEN_WIDTH        128      // Total screen width
#define HUD_SCREEN_HEIGHT       64       // Total screen height
#define WORLD_AREA_HEIGHT       48       // Raycasting area (0-47)
#define HUD_AREA_START          48       // HUD start y (48-63)
#define HUD_AREA_HEIGHT         16       // HUD area height

// HUD element positions (HUD area is y=48..63)
#define HUD_HEALTH_X            2        // Health display X (left side)
#define HUD_HEALTH_Y            50       // Health label Y
#define HUD_AMMO_X              95       // Ammo display X (right-aligned)
#define HUD_AMMO_Y              50       // Ammo label Y
#define HUD_CROSSHAIR_X         64       // Crosshair center X (screen center)
#define HUD_CROSSHAIR_Y         32       // Crosshair center Y (screen center)

// HP bar dimensions (Phase 3.3: improved from 8×3 to 10×4)
#define HP_BAR_WIDTH            10       // Segments (was 8)
#define HP_BAR_HEIGHT           4        // Pixels tall (was 3)
#define HP_BAR_SEGMENT_COUNT    10       // Number of visual segments (was 8)

// Visual thresholds
#define HEALTH_WARNING_THRESHOLD 35      // % health to start blinking (Phase 3.5: 35% gives clearer danger signal)
#define BLINK_INTERVAL_MS       500      // Blink period (500ms on, 500ms off)

}  // namespace DoomHUD

#endif  // DOOM_HUD_H
