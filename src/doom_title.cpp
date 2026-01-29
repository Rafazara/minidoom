#include "doom_title.h"
#include <Arduino.h>
#include <string.h>

namespace DoomTitle {

// Screen dimensions
static const uint8_t TITLE_SCREEN_WIDTH  = 128;
static const uint8_t TITLE_SCREEN_HEIGHT = 64;

// Blink interval for "PRESS FIRE"
static const uint32_t TITLE_BLINK_INTERVAL_MS = 500;

// Simple 3x5 font for title text (A-Z, space, limited set)
// We only define characters that we actually use: M, I, N, D, O, P, R, E, S, F, space.

// 3x5 bitmap rows: bit2..bit0 -> pixels left..right
static const uint8_t FONT_M[5] = {0b101, 0b111, 0b101, 0b101, 0b101};
static const uint8_t FONT_I[5] = {0b111, 0b010, 0b010, 0b010, 0b111};
static const uint8_t FONT_N[5] = {0b101, 0b111, 0b111, 0b111, 0b101};
static const uint8_t FONT_D[5] = {0b110, 0b101, 0b101, 0b101, 0b110};
static const uint8_t FONT_O[5] = {0b111, 0b101, 0b101, 0b101, 0b111};
static const uint8_t FONT_P[5] = {0b111, 0b101, 0b111, 0b100, 0b100};
static const uint8_t FONT_R[5] = {0b110, 0b101, 0b110, 0b101, 0b101};
static const uint8_t FONT_E[5] = {0b111, 0b100, 0b111, 0b100, 0b111};
static const uint8_t FONT_S[5] = {0b111, 0b100, 0b111, 0b001, 0b111};
static const uint8_t FONT_F[5] = {0b111, 0b100, 0b111, 0b100, 0b100};

static bool g_initialized = false;

// Set single pixel in SSD1306-compatible framebuffer
static void setPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (!framebuffer) return;
    if (x >= TITLE_SCREEN_WIDTH || y >= TITLE_SCREEN_HEIGHT) return;

    uint16_t byte_idx = (y / 8) * TITLE_SCREEN_WIDTH + x;
    uint8_t bit_idx = y % 8;
    framebuffer[byte_idx] |= (1 << bit_idx);
}

// Draw a 3x5 character at (x,y)
static void drawChar(uint8_t* framebuffer, uint8_t x, uint8_t y, char c) {
    if (!framebuffer) return;

    const uint8_t* pattern = nullptr;

    switch (c) {
        case 'M': pattern = FONT_M; break;
        case 'I': pattern = FONT_I; break;
        case 'N': pattern = FONT_N; break;
        case 'D': pattern = FONT_D; break;
        case 'O': pattern = FONT_O; break;
        case 'P': pattern = FONT_P; break;
        case 'R': pattern = FONT_R; break;
        case 'E': pattern = FONT_E; break;
        case 'S': pattern = FONT_S; break;
        case 'F': pattern = FONT_F; break;
        case ' ': default: pattern = nullptr; break;
    }

    if (!pattern) {
        return;  // Skip unsupported chars and spaces
    }

    for (uint8_t py = 0; py < 5; ++py) {
        uint8_t row = pattern[py];
        for (uint8_t px = 0; px < 3; ++px) {
            if (row & (1 << (2 - px))) {
                setPixel(framebuffer, x + px, y + py);
            }
        }
    }
}

// Draw a text string using 3x5 font, with 1px spacing between chars
static void drawText(uint8_t* framebuffer, uint8_t x, uint8_t y, const char* text) {
    if (!framebuffer || !text) return;

    uint8_t cursor_x = x;
    for (const char* p = text; *p != '\0'; ++p) {
        if (*p == ' ') {
            cursor_x += 4;  // advance for space
            continue;
        }

        drawChar(framebuffer, cursor_x, y, *p);
        cursor_x += 4;  // 3px char + 1px gap
    }
}

// Compute horizontal start x to center a string with 3x5 font
static uint8_t computeCenteredX(const char* text) {
    if (!text) return 0;

    uint8_t length = 0;
    for (const char* p = text; *p != '\0'; ++p) {
        // Each non-space char takes 4px (3 + 1 gap), space also advances 4 for balance
        length += 4;
    }
    if (length > 0) {
        length -= 1;  // remove last extra gap
    }

    if (length >= TITLE_SCREEN_WIDTH) {
        return 0;
    }

    return (TITLE_SCREEN_WIDTH - length) / 2;
}

// Decide if blinking text should be visible at given time
static bool isBlinkVisible(uint32_t current_time_ms) {
    uint32_t cycle = current_time_ms % (TITLE_BLINK_INTERVAL_MS * 2);
    return cycle < TITLE_BLINK_INTERVAL_MS;
}

void initialize() {
    if (g_initialized) {
        return;
    }

    Serial.println(F("[TITLE] Initialized"));
    g_initialized = true;
}

void render(uint8_t* framebuffer, uint32_t current_time_ms) {
    if (!g_initialized || !framebuffer) {
        return;
    }

    // Clear entire framebuffer (black background)
    memset(framebuffer, 0, TITLE_SCREEN_WIDTH * (TITLE_SCREEN_HEIGHT / 8));

    // Log rendering (throttled to avoid spam)
    static uint32_t last_log_time = 0;
    if (current_time_ms - last_log_time > 1000) {
        Serial.println(F("[TITLE] Rendering title screen"));
        last_log_time = current_time_ms;
    }

    // Draw title text "MINI DOOM" near top (y ≈ 18)
    const char* title_text = "MINI DOOM";
    uint8_t title_x = computeCenteredX(title_text);
    uint8_t title_y = 18;
    drawText(framebuffer, title_x, title_y, title_text);

    // Blinking "PRESS FIRE" near bottom (y ≈ 40)
    const char* prompt_text = "PRESS FIRE";
    if (isBlinkVisible(current_time_ms)) {
        uint8_t prompt_x = computeCenteredX(prompt_text);
        uint8_t prompt_y = 40;
        drawText(framebuffer, prompt_x, prompt_y, prompt_text);

        // Optional low-frequency log when visible
        static uint32_t last_visible_log = 0;
        if (current_time_ms - last_visible_log > 1000) {
            Serial.println(F("[TITLE] PRESS FIRE visible"));
            last_visible_log = current_time_ms;
        }
    }
}

bool shouldStartGame(bool fire_pressed) {
    // Caller provides edge-detected FIRE state: true only when just pressed
    return fire_pressed;
}

}  // namespace DoomTitle
