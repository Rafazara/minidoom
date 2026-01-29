#include "doom_graphics.h"
#include "config.h"
#include "oled_ui.h"
#include <Arduino.h>

namespace DoomGraphics {

// ============================================================
// STATIC FRAMEBUFFER (1024 bytes = 128x64 / 8)
// ============================================================

static uint8_t g_framebuffer[DOOM_BUFFER_SIZE];

// ============================================================
// PERFORMANCE TRACKING
// ============================================================

static uint32_t g_frameCount = 0;
static uint32_t g_lastFrameTime = 0;
static uint32_t g_lastFPSTime = 0;
static uint8_t g_currentFPS = 0;
static uint8_t g_currentPattern = 0;  // 0=checkerboard, 1=scrolling, 2=bars, 3=grid

// ============================================================
// INITIALIZATION
// ============================================================

void initialize() {
    g_frameCount = 0;
    g_lastFrameTime = millis();
    g_lastFPSTime = millis();
    g_currentFPS = 0;
    g_currentPattern = 0;
    
    clearBuffer();
    
    Serial.printf("[mini-doom] [DOOM-GFX] Graphics system initialized\n");
    Serial.printf("[mini-doom] [DOOM-GFX] Resolution: 128x64\n");
    Serial.printf("[mini-doom] [DOOM-GFX] Framebuffer: %d bytes\n", DOOM_BUFFER_SIZE);
    Serial.printf("[mini-doom] [DOOM-GFX] Target FPS: %d\n", DOOM_TARGET_FPS);
    Serial.printf("[mini-doom] [DOOM-GFX] Frame delay: %dms\n", DOOM_FRAME_DELAY_MS);
}

// ============================================================
// FRAMEBUFFER MANAGEMENT
// ============================================================

uint8_t* getFramebuffer() {
    return g_framebuffer;
}

void clearBuffer() {
    memset(g_framebuffer, 0, DOOM_BUFFER_SIZE);
}

void blitBufferToOLED() {
    // Get reference to the OLED display object
    Adafruit_SSD1306* displayPtr = OLEDUI::getDisplay();
    if (!displayPtr) return;
    
    // Send framebuffer to OLED display using Adafruit's drawBitmap
    // The SSD1306 display memory is organized as 8 pages (rows) of 128 bytes each
    // Each byte contains 8 vertical pixels
    displayPtr->clearDisplay();
    displayPtr->drawBitmap(0, 0, g_framebuffer, DOOM_WIDTH, DOOM_HEIGHT, SSD1306_WHITE);
    displayPtr->display();
}

// ============================================================
// PIXEL OPERATIONS
// ============================================================

void setPixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= DOOM_WIDTH || y >= DOOM_HEIGHT) return;
    
    // SSD1306 page layout: 8 vertical pixels per byte
    // byte_index = page_row * 128 + x_column
    uint16_t byte_index = (y / 8) * DOOM_WIDTH + x;
    uint8_t bit_index = 7 - (y & 7);  // MSB-first vertical addressing
    
    if (color) {
        g_framebuffer[byte_index] |= (1 << bit_index);
    } else {
        g_framebuffer[byte_index] &= ~(1 << bit_index);
    }
}

uint8_t getPixel(uint8_t x, uint8_t y) {
    if (x >= DOOM_WIDTH || y >= DOOM_HEIGHT) return 0;
    
    // SSD1306 page layout: 8 vertical pixels per byte
    uint16_t byte_index = (y / 8) * DOOM_WIDTH + x;
    uint8_t bit_index = 7 - (y & 7);  // MSB-first vertical addressing
    
    return (g_framebuffer[byte_index] >> bit_index) & 1;
}

// ============================================================
// LINE DRAWING
// ============================================================

void drawHLine(uint8_t x0, uint8_t y, uint8_t length, uint8_t color) {
    if (y >= DOOM_HEIGHT) return;
    
    for (uint8_t i = 0; i < length && (x0 + i) < DOOM_WIDTH; i++) {
        setPixel(x0 + i, y, color);
    }
}

void drawVLine(uint8_t x, uint8_t y0, uint8_t length, uint8_t color) {
    if (x >= DOOM_WIDTH) return;
    
    for (uint8_t i = 0; i < length && (y0 + i) < DOOM_HEIGHT; i++) {
        setPixel(x, y0 + i, color);
    }
}

// ============================================================
// RECTANGLE FILLING
// ============================================================

void fillRect(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t color) {
    for (uint8_t y = 0; y < height && (y0 + y) < DOOM_HEIGHT; y++) {
        drawHLine(x0, y0 + y, width, color);
    }
}

// ============================================================
// TEST PATTERNS
// ============================================================

void drawCheckboard() {
    clearBuffer();
    
    // 8x8 pixel squares
    uint8_t square_size = 8;
    for (uint8_t y = 0; y < DOOM_HEIGHT; y += square_size) {
        for (uint8_t x = 0; x < DOOM_WIDTH; x += square_size) {
            uint8_t row = y / square_size;
            uint8_t col = x / square_size;
            uint8_t color = ((row + col) % 2) ? 1 : 0;
            
            fillRect(x, y, square_size, square_size, color);
        }
    }
}

void drawScrollingLines(uint16_t frame) {
    clearBuffer();
    
    // Horizontal scrolling lines
    uint8_t offset = frame % DOOM_WIDTH;
    for (uint8_t y = 0; y < DOOM_HEIGHT; y += 4) {
        for (uint8_t x = 0; x < DOOM_WIDTH; x++) {
            uint8_t pos = (x + offset) % DOOM_WIDTH;
            uint8_t color = (pos < 8) ? 1 : 0;
            setPixel(x, y, color);
        }
    }
    
    // Vertical scrolling lines
    offset = frame % DOOM_HEIGHT;
    for (uint8_t x = 0; x < DOOM_WIDTH; x += 4) {
        for (uint8_t y = 0; y < DOOM_HEIGHT; y++) {
            uint8_t pos = (y + offset) % DOOM_HEIGHT;
            uint8_t color = (pos < 8) ? 1 : 0;
            setPixel(x, y, color);
        }
    }
}

void drawAnimatedBars(uint16_t frame) {
    clearBuffer();
    
    // Vertical bars animation
    uint8_t bar_width = 16;
    uint8_t offset = frame % (bar_width * 2);
    
    for (uint8_t x = 0; x < DOOM_WIDTH; x++) {
        uint8_t local_pos = (x + offset) % (bar_width * 2);
        uint8_t color = (local_pos < bar_width) ? 1 : 0;
        
        drawVLine(x, 0, DOOM_HEIGHT, color);
    }
}

void drawGrid(uint8_t spacing) {
    clearBuffer();
    
    // Horizontal lines
    for (uint8_t y = 0; y < DOOM_HEIGHT; y += spacing) {
        drawHLine(0, y, DOOM_WIDTH, 1);
    }
    
    // Vertical lines
    for (uint8_t x = 0; x < DOOM_WIDTH; x += spacing) {
        drawVLine(x, 0, DOOM_HEIGHT, 1);
    }
}

// ============================================================
// PERFORMANCE METRICS
// ============================================================

uint8_t getFPS() {
    return g_currentFPS;
}

uint32_t getFrameCount() {
    return g_frameCount;
}

void resetPerformance() {
    g_frameCount = 0;
    g_lastFPSTime = millis();
    g_currentFPS = 0;
}

void updateFrameTime() {
    g_frameCount++;
    
    uint32_t now = millis();
    uint32_t elapsed = now - g_lastFPSTime;
    
    // Update FPS every second
    if (elapsed >= 1000) {
        g_currentFPS = (g_frameCount * 1000) / elapsed;
        g_frameCount = 0;
        g_lastFPSTime = now;
    }
}

// ============================================================
// ANIMATION & RENDERING LOOP
// ============================================================

bool renderFrame() {
    uint32_t now = millis();
    uint32_t elapsed = now - g_lastFrameTime;
    
    // FPS limiting
    if (elapsed < DOOM_FRAME_DELAY_MS) {
        return false;  // Not ready for next frame yet
    }
    
    g_lastFrameTime = now;
    
    // Render current pattern
    switch (g_currentPattern) {
        case 0:
            drawCheckboard();
            break;
        case 1:
            drawScrollingLines(g_frameCount);
            break;
        case 2:
            drawAnimatedBars(g_frameCount);
            break;
        case 3:
            drawGrid(8);
            break;
        default:
            clearBuffer();
    }
    
    // Send to OLED
    blitBufferToOLED();
    
    // Update performance counter
    updateFrameTime();
    
    return true;
}

uint8_t getCurrentPattern() {
    return g_currentPattern;
}

void nextPattern() {
    g_currentPattern = (g_currentPattern + 1) % 4;
    Serial.printf("[mini-doom] [DOOM-GFX] Pattern changed to: %d\n", g_currentPattern);
}

}  // namespace DoomGraphics
