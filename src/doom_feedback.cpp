#include "doom_feedback.h"
#include <Arduino.h>
#include <string.h>

namespace DoomFeedback {

// Screen constants
static const uint8_t FB_WIDTH  = 128;
static const uint8_t FB_HEIGHT = 64;

// Internal state
static bool g_initialized = false;
static bool g_damage_active = false;
static bool g_hit_confirm_active = false;

enum DamageDirection {
    DAMAGE_NONE = 0,
    DAMAGE_FRONT,
    DAMAGE_BACK,
    DAMAGE_LEFT,
    DAMAGE_RIGHT
};

static DamageDirection g_damage_direction = DAMAGE_NONE;

// Set a single pixel in SSD1306-style framebuffer
static inline void setPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (!framebuffer) return;
    if (x >= FB_WIDTH || y >= FB_HEIGHT) return;

    uint16_t byte_idx = (y / 8) * FB_WIDTH + x;
    uint8_t bit_idx = y % 8;
    framebuffer[byte_idx] |= (1 << bit_idx);
}

// Compute damage direction based on world positions
static DamageDirection computeDirection(
    double player_x,
    double player_y,
    double enemy_x,
    double enemy_y
) {
    double dx = enemy_x - player_x;
    double dy = enemy_y - player_y;

    // If very small vector, treat as front
    double abs_dx = (dx >= 0.0) ? dx : -dx;
    double abs_dy = (dy >= 0.0) ? dy : -dy;
    if (abs_dx < 0.001 && abs_dy < 0.001) {
        return DAMAGE_FRONT;
    }

    if (abs_dx > abs_dy) {
        // Horizontal dominance
        return (dx > 0.0) ? DAMAGE_RIGHT : DAMAGE_LEFT;
    } else {
        // Vertical dominance
        return (dy > 0.0) ? DAMAGE_BACK : DAMAGE_FRONT;
    }
}

// Draw a tiny edge indicator based on direction
static void drawDirectionIndicator(uint8_t* framebuffer, DamageDirection dir) {
    if (!framebuffer) return;

    switch (dir) {
        case DAMAGE_FRONT:
            // Small bar at top center
            for (uint8_t x = 60; x <= 67; ++x) {
                setPixel(framebuffer, x, 2);
            }
            break;

        case DAMAGE_BACK:
            // Small bar at bottom center
            for (uint8_t x = 60; x <= 67; ++x) {
                setPixel(framebuffer, x, FB_HEIGHT - 3);
            }
            break;

        case DAMAGE_LEFT:
            // Small vertical bar at left center
            for (uint8_t y = 28; y <= 35; ++y) {
                setPixel(framebuffer, 2, y);
            }
            break;

        case DAMAGE_RIGHT:
            // Small vertical bar at right center
            for (uint8_t y = 28; y <= 35; ++y) {
                setPixel(framebuffer, FB_WIDTH - 3, y);
            }
            break;

        default:
            break;
    }
}

// Draw a minimal vignette (corners only)
static void drawDamageVignette(uint8_t* framebuffer) {
    if (!framebuffer) return;

    // Top-left corner
    setPixel(framebuffer, 0, 0);
    setPixel(framebuffer, 1, 0);
    setPixel(framebuffer, 0, 1);

    // Top-right corner
    setPixel(framebuffer, FB_WIDTH - 1, 0);
    setPixel(framebuffer, FB_WIDTH - 2, 0);
    setPixel(framebuffer, FB_WIDTH - 1, 1);

    // Bottom-left corner
    setPixel(framebuffer, 0, FB_HEIGHT - 1);
    setPixel(framebuffer, 1, FB_HEIGHT - 1);
    setPixel(framebuffer, 0, FB_HEIGHT - 2);

    // Bottom-right corner
    setPixel(framebuffer, FB_WIDTH - 1, FB_HEIGHT - 1);
    setPixel(framebuffer, FB_WIDTH - 2, FB_HEIGHT - 1);
    setPixel(framebuffer, FB_WIDTH - 1, FB_HEIGHT - 2);
}

// Draw 3x3 flash at crosshair center
static void drawHitConfirm(uint8_t* framebuffer) {
    if (!framebuffer) return;

    uint8_t cx = 64;
    uint8_t cy = 32;

    for (int8_t dy = -1; dy <= 1; ++dy) {
        for (int8_t dx = -1; dx <= 1; ++dx) {
            setPixel(framebuffer, (uint8_t)(cx + dx), (uint8_t)(cy + dy));
        }
    }
}

void initialize() {
    if (g_initialized) {
        return;
    }

    Serial.println(F("[FEEDBACK] Initialized"));
    g_initialized = true;
}

void triggerPlayerDamage(
    double player_x,
    double player_y,
    double enemy_x,
    double enemy_y
) {
    if (!g_initialized) {
        return;
    }

    g_damage_active = true;
    g_damage_direction = computeDirection(player_x, player_y, enemy_x, enemy_y);

    const char* dir_str = "UNKNOWN";
    switch (g_damage_direction) {
        case DAMAGE_FRONT: dir_str = "FRONT"; break;
        case DAMAGE_BACK:  dir_str = "BACK";  break;
        case DAMAGE_LEFT:  dir_str = "LEFT";  break;
        case DAMAGE_RIGHT: dir_str = "RIGHT"; break;
        default: break;
    }

    Serial.printf("[PLAYER-FX] Damage direction: %s\r\n", dir_str);
}

void triggerHitConfirm() {
    if (!g_initialized) {
        return;
    }

    g_hit_confirm_active = true;
    Serial.println(F("[COMBAT-FX] Hit confirm"));
}

void render(uint8_t* framebuffer) {
    if (!g_initialized || !framebuffer) {
        return;
    }

    // Damage direction + vignette (1 frame)
    if (g_damage_active && g_damage_direction != DAMAGE_NONE) {
        drawDirectionIndicator(framebuffer, g_damage_direction);
        drawDamageVignette(framebuffer);
    }

    // Hit confirm flash at crosshair (1 frame)
    if (g_hit_confirm_active) {
        drawHitConfirm(framebuffer);
    }

    // All FX are 1-frame duration
    g_damage_active = false;
    g_damage_direction = DAMAGE_NONE;
    g_hit_confirm_active = false;
}

}  // namespace DoomFeedback

