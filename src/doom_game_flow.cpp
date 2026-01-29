#include "doom_game_flow.h"
#include <Arduino.h>
#include <string.h>

namespace DoomGameFlow {

// Screen constants
static const uint8_t FB_WIDTH  = 128;
static const uint8_t FB_HEIGHT = 64;

// Timing constants
static const uint32_t WAVE_START_TEXT_MS = 1000;
static const uint32_t WAVE_CLEAR_TEXT_MS = 1000;
static const uint32_t WAVE_COOLDOWN_MS   = 2000;
static const uint32_t CLEAR_BLINK_PHASE_MS = 250;  // 2 blinks over 1s (show/hide/show/hide)

enum WaveState {
    WAVE_IDLE = 0,
    WAVE_ACTIVE_PENDING_SPAWN,
    WAVE_ACTIVE,
    WAVE_CLEAR,
    WAVE_COOLDOWN
};

static bool g_initialized = false;
static WaveState g_state = WAVE_IDLE;
static uint8_t g_wave = 0;
static uint8_t g_enemies_remaining = 0;
static bool g_spawn_pending = false;

static uint32_t g_wave_state_start_ms = 0;
static uint32_t g_wave_start_text_start_ms = 0;

// ----------------------------------------------------------------------------
// Minimal 3x5 font (only required chars)
// ----------------------------------------------------------------------------

static const uint8_t DIGITS[10][5] = {
    {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
    {0b001, 0b001, 0b001, 0b001, 0b001}, // 1
    {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
    {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
    {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
    {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
    {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
    {0b111, 0b001, 0b001, 0b001, 0b001}, // 7
    {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
    {0b111, 0b101, 0b111, 0b001, 0b111}, // 9
};

static const uint8_t FONT_W[5] = {0b101, 0b101, 0b101, 0b111, 0b101};
static const uint8_t FONT_A[5] = {0b010, 0b101, 0b111, 0b101, 0b101};
static const uint8_t FONT_V[5] = {0b101, 0b101, 0b101, 0b101, 0b010};
static const uint8_t FONT_E[5] = {0b111, 0b100, 0b111, 0b100, 0b111};
static const uint8_t FONT_C[5] = {0b111, 0b100, 0b100, 0b100, 0b111};
static const uint8_t FONT_L[5] = {0b100, 0b100, 0b100, 0b100, 0b111};
static const uint8_t FONT_R[5] = {0b110, 0b101, 0b110, 0b101, 0b101};

static inline void setPixel(uint8_t* framebuffer, uint8_t x, uint8_t y) {
    if (!framebuffer) return;
    if (x >= FB_WIDTH || y >= FB_HEIGHT) return;

    uint16_t byte_idx = (y / 8) * FB_WIDTH + x;
    uint8_t bit_idx = y % 8;
    framebuffer[byte_idx] |= (1 << bit_idx);
}

static void drawChar(uint8_t* framebuffer, uint8_t x, uint8_t y, char c) {
    const uint8_t* pattern = nullptr;

    if (c >= '0' && c <= '9') {
        pattern = DIGITS[c - '0'];
    } else {
        switch (c) {
            case 'W': pattern = FONT_W; break;
            case 'A': pattern = FONT_A; break;
            case 'V': pattern = FONT_V; break;
            case 'E': pattern = FONT_E; break;
            case 'C': pattern = FONT_C; break;
            case 'L': pattern = FONT_L; break;
            case 'R': pattern = FONT_R; break;
            case ' ': default: pattern = nullptr; break;
        }
    }

    if (!pattern) return;

    for (uint8_t py = 0; py < 5; ++py) {
        uint8_t row = pattern[py];
        for (uint8_t px = 0; px < 3; ++px) {
            if (row & (1 << (2 - px))) {
                setPixel(framebuffer, x + px, y + py);
            }
        }
    }
}

static void drawText(uint8_t* framebuffer, uint8_t x, uint8_t y, const char* text) {
    uint8_t cursor_x = x;
    for (const char* p = text; p && *p; ++p) {
        if (*p == ' ') {
            cursor_x += 4;
            continue;
        }
        drawChar(framebuffer, cursor_x, y, *p);
        cursor_x += 4;
    }
}

static uint8_t computeCenteredX(const char* text) {
    if (!text) return 0;
    uint8_t length = 0;
    for (const char* p = text; *p; ++p) {
        length += 4;
    }
    if (length > 0) length -= 1;
    if (length >= FB_WIDTH) return 0;
    return (FB_WIDTH - length) / 2;
}

// ----------------------------------------------------------------------------
// Wave state helpers
// ----------------------------------------------------------------------------

static void startWave(uint8_t wave_number, uint32_t current_time_ms) {
    g_wave = wave_number;
    g_state = WAVE_ACTIVE_PENDING_SPAWN;
    g_spawn_pending = true;
    g_wave_state_start_ms = current_time_ms;
    g_wave_start_text_start_ms = current_time_ms;

    Serial.printf("[FLOW] Wave %d started\r\n", g_wave);
}

void initialize() {
    // Reset internal state every time (used for restarts),
    // but only log "Initialized" on first call.
    if (!g_initialized) {
        Serial.println(F("[FLOW] Initialized"));
        g_initialized = true;
    }

    g_state = WAVE_IDLE;
    g_wave = 0;
    g_enemies_remaining = 0;
    g_spawn_pending = false;
    g_wave_state_start_ms = 0;
    g_wave_start_text_start_ms = 0;
}

void update(uint32_t current_time_ms) {
    if (!g_initialized) return;

    switch (g_state) {
        case WAVE_IDLE:
            // Start wave 1 automatically when entering PLAYING
            if (g_wave == 0) {
                startWave(1, current_time_ms);
            }
            break;

        case WAVE_ACTIVE_PENDING_SPAWN:
            // Wait for adapter to spawn on shouldSpawnWave()
            break;

        case WAVE_ACTIVE:
            // Wait for notifyEnemyKilled() to drive transitions
            break;

        case WAVE_CLEAR: {
            uint32_t elapsed = current_time_ms - g_wave_state_start_ms;
            if (elapsed >= WAVE_CLEAR_TEXT_MS) {
                g_state = WAVE_COOLDOWN;
                g_wave_state_start_ms = current_time_ms;
                Serial.println(F("[FLOW] Cooldown..."));
            }
            break;
        }

        case WAVE_COOLDOWN: {
            uint32_t elapsed = current_time_ms - g_wave_state_start_ms;
            if (elapsed >= WAVE_COOLDOWN_MS) {
                startWave((uint8_t)(g_wave + 1), current_time_ms);
            }
            break;
        }

        default:
            break;
    }
}

void notifyEnemyKilled() {
    if (!g_initialized) return;
    if (g_state != WAVE_ACTIVE) return;

    if (g_enemies_remaining > 0) {
        g_enemies_remaining--;
    }

    Serial.printf("[FLOW] Enemy killed (remaining: %d)\r\n", g_enemies_remaining);

    if (g_enemies_remaining == 0) {
        g_state = WAVE_CLEAR;
        g_wave_state_start_ms = millis();
        Serial.printf("[FLOW] Wave %d cleared\r\n", g_wave);
    }
}

bool shouldSpawnWave() {
    if (!g_initialized) return false;
    if (!g_spawn_pending) return false;

    // One-shot gate
    g_spawn_pending = false;

    // Wave becomes active now; set enemy counter
    g_enemies_remaining = getEnemyCountForWave();
    g_state = WAVE_ACTIVE;

    return true;
}

uint8_t getCurrentWave() {
    return g_wave;
}

uint8_t getEnemyCountForWave() {
    if (g_wave == 0) return 0;
    // Phase 3.5: Tuned difficulty curve - linear scaling prevents sudden spikes
    // Wave 1: 3 enemies, Wave 2: 4, Wave 3: 5, etc.
    // Structure ready for asymmetric scaling if needed in future phases
    return (uint8_t)(2 + g_wave);
}

void render(uint8_t* framebuffer, uint32_t current_time_ms) {
    if (!g_initialized || !framebuffer) return;

    // Wave start text (for ~1s after wave start)
    if (g_state == WAVE_ACTIVE || g_state == WAVE_ACTIVE_PENDING_SPAWN) {
        uint32_t elapsed = current_time_ms - g_wave_start_text_start_ms;
        if (elapsed < WAVE_START_TEXT_MS) {
            char buf[12];
            // "WAVE " + up to 3 digits
            snprintf(buf, sizeof(buf), "WAVE %u", (unsigned)g_wave);
            uint8_t x = computeCenteredX(buf);
            drawText(framebuffer, x, 2, buf);
        }
    }

    // Wave clear text "CLEAR" blinking twice over ~1s
    if (g_state == WAVE_CLEAR) {
        uint32_t elapsed = current_time_ms - g_wave_state_start_ms;
        if (elapsed < WAVE_CLEAR_TEXT_MS) {
            uint8_t phase = (uint8_t)(elapsed / CLEAR_BLINK_PHASE_MS); // 0..3
            bool visible = (phase == 0 || phase == 2);
            if (visible) {
                const char* text = "CLEAR";
                uint8_t x = computeCenteredX(text);
                drawText(framebuffer, x, 2, text);
            }
        }
    }
}

// Phase 3.5: Return enemies remaining for balance logging
uint8_t getEnemiesRemaining() {
    return g_enemies_remaining;
}

}  // namespace DoomGameFlow

