/**
 * Doom HUD System - Phase 3.3 Implementation
 * 
 * Polish & Visual Hierarchy - Enhanced HUD rendering with improved clarity and Doom aesthetics
 * Displays health, ammo, and crosshair with visual hierarchy and instant readability
 * 
 * Key Improvements from Phase 2.11:
 * - Health bar: 8×3 → 10×4 pixels (thicker, more prominent)
 * - Health number: Now larger and positioned directly below bar
 * - Ammo layout: Clear label + large number format
 * - Crosshair: 3×3 solid → 5×5 hollow (better visibility on textures)
 * - Separator: Horizontal line at y=47 (visual foundation)
 * - Visual hierarchy: Player reads HP/Ammo instantly, crosshair doesn't interfere
 * 
 * Performance: <1ms per frame (only small pixel loops, no depth buffer access)
 */

#include "doom_hud.h"
#include <Arduino.h>
#include <string.h>
#include <math.h>

namespace DoomHUD {

// ============================================================================
// MODULE STATE
// ============================================================================

static bool g_initialized = false;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Check if should blink based on current time
 * Used for low health and no ammo warnings
 * 
 * @param current_time_ms: Current system time
 * @param blink_interval: Interval in milliseconds
 * @return true if in "on" phase of blink cycle, false if "off"
 */
static bool shouldBlink(uint32_t current_time_ms, uint32_t blink_interval) {
    uint32_t cycle = current_time_ms % (blink_interval * 2);
    return cycle < blink_interval;
}

/**
 * Set a pixel in the framebuffer
 * Framebuffer is 128×64 / 8 = 1024 bytes
 * Each byte represents 8 vertical pixels
 * 
 * @param framebuffer: Framebuffer pointer
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 */
static void setPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (x >= HUD_SCREEN_WIDTH || y >= HUD_SCREEN_HEIGHT) return;
    
    // SSD1306-compatible layout: 8-pixel vertical pages, 128 bytes per page
    uint16_t byte_idx = (y / 8) * HUD_SCREEN_WIDTH + x;
    uint8_t bit_idx = y % 8;
    
    framebuffer[byte_idx] |= (1 << bit_idx);
}

/**
 * Clear a pixel in the framebuffer
 */
static void clearPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (x >= HUD_SCREEN_WIDTH || y >= HUD_SCREEN_HEIGHT) return;
    
    uint16_t byte_idx = (y / 8) * HUD_SCREEN_WIDTH + x;
    uint8_t bit_idx = y % 8;
    
    framebuffer[byte_idx] &= ~(1 << bit_idx);
}

/**
 * Draw a horizontal line in framebuffer (for HUD separator)
 * @param framebuffer: Framebuffer pointer
 * @param x_start: Starting X
 * @param x_end: Ending X
 * @param y: Y coordinate
 */
static void drawHorizontalLine(uint8_t* framebuffer, uint8_t x_start, uint8_t x_end, uint8_t y) {
    if (y >= HUD_SCREEN_HEIGHT) return;
    for (uint8_t x = x_start; x <= x_end && x < HUD_SCREEN_WIDTH; x++) {
        setPixel(framebuffer, x, y);
    }
}

/**
 * Draw a character (3×5 pixels) in the framebuffer
 * Simple font: only supports numbers and basic text
 * 
 * Character encoding (3×5 bitmap):
 * 0:  ###
 *     # #
 *     # #
 *     # #
 *     ###
 * 
 * 1:  #
 *     #
 *     #
 *     #
 *     #
 */
static void drawChar(uint8_t* framebuffer, uint8_t x, uint8_t y, char c) {
    // Simple 3×5 monospace font - minimal set for HUD
    // Supports digits 0-9 and a few letters needed for HUD labels
    
    static const uint8_t DIGIT_PATTERNS[10][5] = {
        // 0: ###
        //     # #
        //     # #
        //     # #
        //     ###
        {0b111, 0b101, 0b101, 0b101, 0b111},
        
        // 1: #
        //     #
        //     #
        //     #
        //     #
        {0b001, 0b001, 0b001, 0b001, 0b001},
        
        // 2: ###
        //       #
        //     ###
        //     #
        //     ###
        {0b111, 0b001, 0b111, 0b100, 0b111},
        
        // 3: ###
        //       #
        //     ###
        //       #
        //     ###
        {0b111, 0b001, 0b111, 0b001, 0b111},
        
        // 4: # #
        //     # #
        //     ###
        //       #
        //       #
        {0b101, 0b101, 0b111, 0b001, 0b001},
        
        // 5: ###
        //     #
        //     ###
        //       #
        //     ###
        {0b111, 0b100, 0b111, 0b001, 0b111},
        
        // 6: ###
        //     #
        //     ###
        //     # #
        //     ###
        {0b111, 0b100, 0b111, 0b101, 0b111},
        
        // 7: ###
        //       #
        //       #
        //       #
        //       #
        {0b111, 0b001, 0b001, 0b001, 0b001},
        
        // 8: ###
        //     # #
        //     ###
        //     # #
        //     ###
        {0b111, 0b101, 0b111, 0b101, 0b111},
        
        // 9: ###
        //     # #
        //     ###
        //       #
        //     ###
        {0b111, 0b101, 0b111, 0b001, 0b111},
    };
    
    // Minimal letter set (H, P, N, O, A, M)
    static const uint8_t LETTER_H[5] = {0b101, 0b101, 0b111, 0b101, 0b101};
    static const uint8_t LETTER_P[5] = {0b111, 0b101, 0b111, 0b100, 0b100};
    static const uint8_t LETTER_N[5] = {0b101, 0b111, 0b111, 0b111, 0b101};
    static const uint8_t LETTER_O[5] = {0b111, 0b101, 0b101, 0b101, 0b111};
    static const uint8_t LETTER_A[5] = {0b010, 0b101, 0b111, 0b101, 0b101};
    static const uint8_t LETTER_M[5] = {0b101, 0b111, 0b101, 0b101, 0b101};

    const uint8_t* pattern = nullptr;

    if (c >= '0' && c <= '9') {
        pattern = DIGIT_PATTERNS[c - '0'];
    } else {
        switch (c) {
            case 'H': pattern = LETTER_H; break;
            case 'P': pattern = LETTER_P; break;
            case 'N': pattern = LETTER_N; break;
            case 'O': pattern = LETTER_O; break;
            case 'A': pattern = LETTER_A; break;
            case 'M': pattern = LETTER_M; break;
            default:  pattern = nullptr;   break;
        }
    }

    if (!pattern) {
        return;
    }

    // Draw 3×5 character
    for (uint8_t py = 0; py < 5; py++) {
        uint8_t row = pattern[py];
        for (uint8_t px = 0; px < 3; px++) {
            if (row & (1 << (2 - px))) {  // Bit 2, 1, 0
                setPixel(framebuffer, x + px, y + py);
            }
        }
    }
}

/**
 * Draw a health bar (10 pixels wide, 4 pixels tall)
 * Shows visual representation of health percentage with improved prominence
 * 
 * Improvements from 8×3:
 *   - Larger overall (10×4 vs 8×3)
 *   - More visible against background
 *   - 10 segments instead of 8 for finer granularity
 *   - Always bordered on all sides
 * 
 * Layout:
 *   ████████░░  <- Filled solid, empty outline only
 *   ██████░░░░
 *   ████████░░  <- Always bordered (top/bottom/left/right)
 *   ██████░░░░
 * 
 * @param framebuffer: Framebuffer pointer
 * @param x: Starting X coordinate
 * @param y: Starting Y coordinate
 * @param health: Health value (0-100)
 */
static void drawHealthBar(uint8_t* framebuffer, uint8_t x, uint8_t y, uint8_t health) {
    // Calculate filled segments (10 segments total for finer control)
    uint8_t filled = (health * HP_BAR_SEGMENT_COUNT) / 100;
    
    // Draw bar with border on all sides
    // Top border (entire width + left/right margin)
    setPixel(framebuffer, x - 1, y);
    for (uint8_t seg = 0; seg < HP_BAR_WIDTH; seg++) {
        setPixel(framebuffer, x + seg, y);
    }
    setPixel(framebuffer, x + HP_BAR_WIDTH, y);
    
    // Middle rows (3 rows total height)
    for (uint8_t row = 1; row < HP_BAR_HEIGHT - 1; row++) {
        // Left border
        setPixel(framebuffer, x - 1, y + row);
        
        // Fill segments
        for (uint8_t seg = 0; seg < HP_BAR_WIDTH; seg++) {
            if (seg < filled) {
                // Filled segment: solid column
                setPixel(framebuffer, x + seg, y + row);
            } else {
                // Empty segment: right edge only (vertical line between segments)
                if (seg > 0) {
                    setPixel(framebuffer, x + seg, y + row);
                }
            }
        }
        
        // Right border
        setPixel(framebuffer, x + HP_BAR_WIDTH, y + row);
    }
    
    // Bottom border (entire width + left/right margin)
    setPixel(framebuffer, x - 1, y + HP_BAR_HEIGHT - 1);
    for (uint8_t seg = 0; seg < HP_BAR_WIDTH; seg++) {
        setPixel(framebuffer, x + seg, y + HP_BAR_HEIGHT - 1);
    }
    setPixel(framebuffer, x + HP_BAR_WIDTH, y + HP_BAR_HEIGHT - 1);
}

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

void initialize() {
    if (g_initialized) {
        return;
    }
    
    Serial.println(F("[HUD] Initialized (Phase 3.3 - Polish & Visual Hierarchy)"));
    g_initialized = true;
}

bool isInitialized() {
    return g_initialized;
}

void drawHealth(uint8_t* framebuffer, uint8_t health, bool blink) {
    if (!g_initialized) return;
    
    // Skip rendering when blink flag indicates "off" phase
    if (blink) {
        return;
    }
    
    // Draw "HP" label (small, 3×5 each)
    drawChar(framebuffer, HUD_HEALTH_X, HUD_HEALTH_Y, 'H');
    drawChar(framebuffer, HUD_HEALTH_X + 4, HUD_HEALTH_Y, 'P');
    
    // Draw health bar (10×4, improved from 8×3)
    // Position bar directly below label, with some spacing
    drawHealthBar(framebuffer, HUD_HEALTH_X, HUD_HEALTH_Y + 6, health);
    
    // Draw large numeric value below bar (two digits, 4×5 per character)
    // Positioned for instant readability
    uint8_t tens = health / 10;
    uint8_t ones = health % 10;
    
    // Draw tens digit
    if (tens > 0) {
        drawChar(framebuffer, HUD_HEALTH_X, HUD_HEALTH_Y + 11, '0' + tens);
        drawChar(framebuffer, HUD_HEALTH_X + 4, HUD_HEALTH_Y + 11, '0' + ones);
    } else {
        // Single digit: center it
        drawChar(framebuffer, HUD_HEALTH_X + 2, HUD_HEALTH_Y + 11, '0' + ones);
    }
}

void drawAmmo(uint8_t* framebuffer, uint8_t ammo, bool blink) {
    if (!g_initialized) return;
    
    // Skip rendering when blink flag indicates "off" phase
    if (blink && ammo == 0) {
        return;
    }
    
    // Draw ammo label (small "AMMO" text, right-aligned)
    // Position: right side of screen
    // Using sequential characters for "AMMO"
    drawChar(framebuffer, HUD_AMMO_X - 12, HUD_AMMO_Y, 'A');
    drawChar(framebuffer, HUD_AMMO_X - 8, HUD_AMMO_Y, 'M');
    drawChar(framebuffer, HUD_AMMO_X - 4, HUD_AMMO_Y, 'M');
    drawChar(framebuffer, HUD_AMMO_X, HUD_AMMO_Y, 'O');
    
    // Draw ammo value (large two-digit number below label)
    if (ammo == 0) {
        // Special state: "NO AMMO" instead of "00"
        // "NO" (display as "N" and "O" for clarity)
        drawChar(framebuffer, HUD_AMMO_X - 8, HUD_AMMO_Y + 6, 'N');
        drawChar(framebuffer, HUD_AMMO_X - 4, HUD_AMMO_Y + 6, 'O');
    } else {
        // Regular ammo display
        uint8_t tens = ammo / 10;
        uint8_t ones = ammo % 10;
        
        if (tens > 0) {
            // Two digits
            drawChar(framebuffer, HUD_AMMO_X - 4, HUD_AMMO_Y + 6, '0' + tens);
            drawChar(framebuffer, HUD_AMMO_X, HUD_AMMO_Y + 6, '0' + ones);
        } else {
            // Single digit (right-aligned)
            drawChar(framebuffer, HUD_AMMO_X, HUD_AMMO_Y + 6, '0' + ones);
        }
    }
}

void drawCrosshair(uint8_t* framebuffer) {
    if (!g_initialized) return;
    
    // Draw 5×5 hollow crosshair at center of screen
    // Position: center of world view (64, 32)
    
    uint8_t cx = HUD_CROSSHAIR_X;
    uint8_t cy = HUD_CROSSHAIR_Y;
    
    // Top arm (2 pixels, vertical)
    setPixel(framebuffer, cx, cy - 2);
    setPixel(framebuffer, cx, cy - 1);
    
    // Bottom arm (2 pixels, vertical)
    setPixel(framebuffer, cx, cy + 1);
    setPixel(framebuffer, cx, cy + 2);
    
    // Left arm (2 pixels, horizontal)
    setPixel(framebuffer, cx - 2, cy);
    setPixel(framebuffer, cx - 1, cy);
    
    // Right arm (2 pixels, horizontal)
    setPixel(framebuffer, cx + 1, cy);
    setPixel(framebuffer, cx + 2, cy);
    
    // Note: Center pixel (cx, cy) is intentionally LEFT EMPTY
    // This creates the "hollow" effect and improves sight-line visibility
    // Players can see what they're aiming at through the crosshair center
}

void renderHUD(
    uint8_t* framebuffer,
    uint8_t player_health,
    uint8_t player_ammo,
    uint32_t current_time_ms
) {
    if (!g_initialized || !framebuffer) return;
    
    // Calculate blinking states
    bool health_warning = player_health < HEALTH_WARNING_THRESHOLD;
    bool no_ammo = (player_ammo == 0);
    
    bool health_blink = health_warning && shouldBlink(current_time_ms, BLINK_INTERVAL_MS);
    bool ammo_blink = no_ammo && shouldBlink(current_time_ms, BLINK_INTERVAL_MS);
    
    // Clear HUD band (y = 48..63) so 3D world never occupies HUD area
    uint8_t first_page = HUD_AREA_START / 8;            // 48 / 8 = 6
    uint8_t last_page  = (HUD_SCREEN_HEIGHT / 8) - 1;   // 64 / 8 - 1 = 7
    for (uint8_t page = first_page; page <= last_page; ++page) {
        uint16_t base = page * HUD_SCREEN_WIDTH;
        memset(framebuffer + base, 0, HUD_SCREEN_WIDTH);
    }
    
    // Draw HUD separator line (horizontal line at y=47)
    drawHorizontalLine(framebuffer, 0, HUD_SCREEN_WIDTH - 1, HUD_AREA_START - 1);
    
    // Draw health display (left side)
    drawHealth(framebuffer, player_health, health_blink);
    
    // Draw ammo display (right side)
    drawAmmo(framebuffer, player_ammo, ammo_blink);
    
    // Draw crosshair (center)
    drawCrosshair(framebuffer);
    
    // Logging
    static uint32_t last_log_time = 0;
    if (current_time_ms - last_log_time > 1000) {  // Log every 1 second
        Serial.printf("[HUD] HP=%d Ammo=%d\r\n", player_health, player_ammo);
        last_log_time = current_time_ms;
        
        if (health_warning) {
            Serial.println(F("[HUD] Low health warning"));
        }
        if (no_ammo) {
            Serial.println(F("[HUD] No ammo"));
        }
    }
}

}  // namespace DoomHUD
