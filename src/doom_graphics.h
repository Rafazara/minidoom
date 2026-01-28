#ifndef DOOM_GRAPHICS_H
#define DOOM_GRAPHICS_H

#include <stdint.h>
#include <string.h>

// Display dimensions
#define DOOM_WIDTH 128
#define DOOM_HEIGHT 32
#define DOOM_BYTES_PER_ROW (DOOM_WIDTH / 8)  // 16 bytes per row
#define DOOM_BUFFER_SIZE (DOOM_BYTES_PER_ROW * DOOM_HEIGHT)  // 512 bytes

// FPS control
#define DOOM_TARGET_FPS 20
#define DOOM_FRAME_DELAY_MS (1000 / DOOM_TARGET_FPS)  // ~50ms per frame

namespace DoomGraphics {

// ============================================================
// FRAMEBUFFER MANAGEMENT
// ============================================================

/**
 * Initialize graphics system
 * Allocates framebuffer and resets rendering state
 */
void initialize();

/**
 * Get reference to the framebuffer (1-bit format, 128x64)
 * Each byte contains 8 pixels (MSB = leftmost)
 */
uint8_t* getFramebuffer();

/**
 * Clear entire framebuffer (all black)
 */
void clearBuffer();

/**
 * Blit framebuffer to OLED display
 * Transfers the internal framebuffer to the SSD1306 OLED
 */
void blitBufferToOLED();

// ============================================================
// DRAWING PRIMITIVES
// ============================================================

/**
 * Set a single pixel in the framebuffer
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 * @param color: 1 = white (on), 0 = black (off)
 */
void setPixel(uint8_t x, uint8_t y, uint8_t color);

/**
 * Get a single pixel from the framebuffer
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 * @return: 1 if white, 0 if black
 */
uint8_t getPixel(uint8_t x, uint8_t y);

/**
 * Draw a horizontal line
 * @param x0: Start X
 * @param y: Y coordinate
 * @param length: Line length in pixels
 * @param color: 1 = white, 0 = black
 */
void drawHLine(uint8_t x0, uint8_t y, uint8_t length, uint8_t color);

/**
 * Draw a vertical line
 * @param x: X coordinate
 * @param y0: Start Y
 * @param length: Line length in pixels
 * @param color: 1 = white, 0 = black
 */
void drawVLine(uint8_t x, uint8_t y0, uint8_t length, uint8_t color);

/**
 * Fill a rectangle
 * @param x0: Top-left X
 * @param y0: Top-left Y
 * @param width: Rectangle width
 * @param height: Rectangle height
 * @param color: 1 = white, 0 = black
 */
void fillRect(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t color);

// ============================================================
// TEST PATTERNS
// ============================================================

/**
 * Draw checkerboard pattern (8x8 squares)
 * Useful for testing display rendering
 */
void drawCheckboard();

/**
 * Draw animated scrolling lines
 * @param frame: Animation frame number (auto-incremented)
 */
void drawScrollingLines(uint16_t frame);

/**
 * Draw animated vertical bars
 * @param frame: Animation frame number (auto-incremented)
 */
void drawAnimatedBars(uint16_t frame);

/**
 * Draw a simple grid pattern
 * @param spacing: Grid spacing in pixels
 */
void drawGrid(uint8_t spacing);

// ============================================================
// PERFORMANCE METRICS
// ============================================================

/**
 * Get current FPS (calculated from frame times)
 * @return: Frames per second (uint8_t)
 */
uint8_t getFPS();

/**
 * Get total frames rendered since initialization
 * @return: Frame count (uint32_t)
 */
uint32_t getFrameCount();

/**
 * Reset performance counters
 */
void resetPerformance();

/**
 * Update FPS counter (should be called once per frame)
 */
void updateFrameTime();

// ============================================================
// ANIMATION & RENDERING LOOP
// ============================================================

/**
 * Run one frame of graphics rendering
 * Handles animation timing, FPS limiting, and buffer blitting
 * Should be called in main loop
 * @return: true if frame was rendered, false if throttled (waiting for next frame time)
 */
bool renderFrame();

/**
 * Get current render state (useful for debugging)
 * @return: Current animation pattern index (0-3)
 */
uint8_t getCurrentPattern();

/**
 * Cycle to next test pattern
 */
void nextPattern();

}  // namespace DoomGraphics

#endif  // DOOM_GRAPHICS_H
