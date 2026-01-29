#ifndef DOOM_VALIDATION_TEST_H
#define DOOM_VALIDATION_TEST_H

#include <stdint.h>
#include "doom_graphics.h"

/**
 * Doom Render Pipeline Validation Test Suite
 * 
 * Purpose:
 * Generate deterministic, repeatable framebuffer patterns to validate:
 * - SSD1306 OLED display mapping (orientation, bit order)
 * - Framebuffer I/O pipeline (blit, clear, copy)
 * - Individual pixel rendering (X,Y coordinates)
 * - Dithering and intensity levels
 * - HUD text rendering
 * 
 * Non-invasive: Does not modify game state, can run anytime.
 * Each test is <50ms (fits in frame budget).
 * 
 * Usage:
 *   DoomValidationTest::runTest(TEST_PATTERN_CHECKERBOARD);
 *   // Observe OLED for expected pattern
 * 
 * Test patterns produce KNOWN-GOOD output for visual confirmation.
 */

namespace DoomValidationTest {

/**
 * Test pattern enumeration
 */
enum TestPattern {
    // Stage 1: Basic display connectivity
    TEST_PATTERN_ALL_BLACK = 0,     // All pixels off (blank screen)
    TEST_PATTERN_ALL_WHITE = 1,     // All pixels on (solid white)
    
    // Stage 2: Coordinate mapping (display orientation, bit order)
    TEST_PATTERN_PIXEL_0_0 = 2,     // Single pixel at (0, 0) top-left
    TEST_PATTERN_PIXEL_127_0 = 3,   // Single pixel at (127, 0) top-right
    TEST_PATTERN_PIXEL_0_63 = 4,    // Single pixel at (0, 63) bottom-left
    TEST_PATTERN_PIXEL_127_63 = 5,  // Single pixel at (127, 63) bottom-right
    TEST_PATTERN_CENTER = 6,        // Single pixel at (64, 32) center
    
    // Stage 3: Line rendering
    TEST_PATTERN_HORIZONTAL_LINES = 7,   // Horizontal lines every 8 pixels
    TEST_PATTERN_VERTICAL_LINES = 8,     // Vertical lines every 8 pixels
    TEST_PATTERN_GRID = 9,               // 8×8 grid pattern
    
    // Stage 4: Dithering and intensity
    TEST_PATTERN_CHECKERBOARD = 10,  // 2×2 checkerboard (alternating pixels)
    TEST_PATTERN_GRADIENT = 11,      // 5-level gradient (simulates Doom dithering)
    
    // Stage 5: Complete scene (walls, HUD, marker)
    TEST_PATTERN_COMPLETE_SCENE = 12, // Walls, HUD, crosshair
    
    // Stage 6: Animation test
    TEST_PATTERN_SCROLLING_PATTERN = 13, // Moving pattern (8 frames)
};

/**
 * Run a specific test pattern
 * 
 * @param pattern: Test pattern ID (see TestPattern enum)
 * @return true if test succeeded, false if failed
 * 
 * Note: Test writes to game framebuffer but does not modify game state.
 * After test, call DoomGraphics::blitBufferToOLED() to see result.
 */
bool runTest(TestPattern pattern);

/**
 * Get human-readable description of test pattern
 * 
 * @param pattern: Test pattern ID
 * @return Pointer to static string describing pattern
 */
const char* getTestDescription(TestPattern pattern);

/**
 * Get expected visual result for test pattern
 * (Describes what SHOULD appear on OLED)
 * 
 * @param pattern: Test pattern ID
 * @return Pointer to static string describing expected output
 */
const char* getExpectedResult(TestPattern pattern);

/**
 * Helper: Fill entire framebuffer with value
 * 
 * @param framebuffer: Pointer to 1024-byte buffer
 * @param value: 0x00 (all black) or 0xFF (all white)
 */
void fillFramebuffer(uint8_t* framebuffer, uint8_t value);

/**
 * Helper: Set single pixel in framebuffer
 * 
 * @param framebuffer: Pointer to 1024-byte buffer
 * @param x: X coordinate (0-127)
 * @param y: Y coordinate (0-63)
 * @param color: 1 (white) or 0 (black)
 */
void setPixelInBuffer(uint8_t* framebuffer, uint8_t x, uint8_t y, uint8_t color);

/**
 * Helper: Draw horizontal line in framebuffer
 * 
 * @param framebuffer: Pointer to 1024-byte buffer
 * @param x0: Start X
 * @param y: Y coordinate
 * @param length: Length in pixels
 * @param color: 1 (white) or 0 (black)
 */
void drawHLineInBuffer(uint8_t* framebuffer, uint8_t x0, uint8_t y, uint8_t length, uint8_t color);

/**
 * Helper: Draw vertical line in framebuffer
 * 
 * @param framebuffer: Pointer to 1024-byte buffer
 * @param x: X coordinate
 * @param y0: Start Y
 * @param length: Length in pixels
 * @param color: 1 (white) or 0 (black)
 */
void drawVLineInBuffer(uint8_t* framebuffer, uint8_t x, uint8_t y0, uint8_t length, uint8_t color);

/**
 * Helper: Draw rectangle in framebuffer
 * 
 * @param framebuffer: Pointer to 1024-byte buffer
 * @param x0, y0: Top-left corner
 * @param width, height: Dimensions
 * @param color: 1 (white) or 0 (black)
 */
void drawRectInBuffer(uint8_t* framebuffer, uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t color);

}  // namespace DoomValidationTest

#endif  // DOOM_VALIDATION_TEST_H
