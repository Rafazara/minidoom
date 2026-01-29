#include "doom_validation_test.h"
#include <Arduino.h>
#include <string.h>

namespace DoomValidationTest {

// ============================================================================
// TEST PATTERN DESCRIPTIONS & EXPECTED RESULTS
// ============================================================================

static const char* test_descriptions[] = {
    "All pixels OFF (blank screen)",
    "All pixels ON (solid white)",
    "Single pixel at (0,0) - top-left corner",
    "Single pixel at (127,0) - top-right corner",
    "Single pixel at (0,63) - bottom-left corner",
    "Single pixel at (127,63) - bottom-right corner",
    "Single pixel at (64,32) - screen center",
    "Horizontal lines every 8 pixels (rows)",
    "Vertical lines every 8 pixels (columns)",
    "8×8 grid pattern (lines every 8px)",
    "2×2 checkerboard (alternating pixels)",
    "5-level gradient (Doom dithering simulation)",
    "Complete scene: walls + HUD + crosshair",
    "Scrolling pattern (animation, 8 frames)",
};

static const char* expected_results[] = {
    "OLED completely black/dark",
    "OLED completely white/bright",
    "Single bright pixel at TOP-LEFT corner only",
    "Single bright pixel at TOP-RIGHT corner only",
    "Single bright pixel at BOTTOM-LEFT corner only",
    "Single bright pixel at BOTTOM-RIGHT corner only",
    "Single bright pixel at CENTER of screen only",
    "8 horizontal stripes (every 8 pixels), equal spacing",
    "16 vertical stripes (every 8 pixels), equal spacing",
    "Regular grid of 8×8 squares",
    "Alternating black/white pixels in checkerboard pattern",
    "Gradient from top (light) to bottom (dark) with 5 shades",
    "Walls on sides, HUD at bottom, crosshair at center",
    "Pattern slowly moving across screen (or animating)",
};

const char* getTestDescription(TestPattern pattern) {
    if (pattern < 14) {
        return test_descriptions[pattern];
    }
    return "UNKNOWN PATTERN";
}

const char* getExpectedResult(TestPattern pattern) {
    if (pattern < 14) {
        return expected_results[pattern];
    }
    return "UNKNOWN PATTERN";
}

// ============================================================================
// HELPER FUNCTIONS (Framebuffer manipulation)
// ============================================================================

void fillFramebuffer(uint8_t* framebuffer, uint8_t value) {
    if (!framebuffer) return;
    memset(framebuffer, value, 1024);
}

void setPixelInBuffer(uint8_t* framebuffer, uint8_t x, uint8_t y, uint8_t color) {
    if (!framebuffer) return;
    if (x >= 128 || y >= 64) return;
    
    // SSD1306 format: page-based, LSB-first (after bit reversal fix)
    uint16_t byte_index = (y / 8) * 128 + x;
    uint8_t bit_index = 7 - (y % 8);  // MSB-first for display compatibility
    
    if (color) {
        framebuffer[byte_index] |= (1 << bit_index);
    } else {
        framebuffer[byte_index] &= ~(1 << bit_index);
    }
}

void drawHLineInBuffer(uint8_t* framebuffer, uint8_t x0, uint8_t y, uint8_t length, uint8_t color) {
    if (!framebuffer) return;
    for (uint8_t i = 0; i < length && (x0 + i) < 128; i++) {
        setPixelInBuffer(framebuffer, x0 + i, y, color);
    }
}

void drawVLineInBuffer(uint8_t* framebuffer, uint8_t x, uint8_t y0, uint8_t length, uint8_t color) {
    if (!framebuffer) return;
    for (uint8_t i = 0; i < length && (y0 + i) < 64; i++) {
        setPixelInBuffer(framebuffer, x, y0 + i, color);
    }
}

void drawRectInBuffer(uint8_t* framebuffer, uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t color) {
    if (!framebuffer) return;
    
    // Top and bottom edges
    drawHLineInBuffer(framebuffer, x0, y0, width, color);
    if (y0 + height - 1 < 64) {
        drawHLineInBuffer(framebuffer, x0, y0 + height - 1, width, color);
    }
    
    // Left and right edges
    drawVLineInBuffer(framebuffer, x0, y0, height, color);
    if (x0 + width - 1 < 128) {
        drawVLineInBuffer(framebuffer, x0 + width - 1, y0, height, color);
    }
}

// ============================================================================
// TEST PATTERN IMPLEMENTATIONS
// ============================================================================

static bool testAllBlack(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    return true;
}

static bool testAllWhite(uint8_t* fb) {
    fillFramebuffer(fb, 0xFF);
    return true;
}

static bool testPixel_0_0(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    setPixelInBuffer(fb, 0, 0, 1);
    return true;
}

static bool testPixel_127_0(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    setPixelInBuffer(fb, 127, 0, 1);
    return true;
}

static bool testPixel_0_63(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    setPixelInBuffer(fb, 0, 63, 1);
    return true;
}

static bool testPixel_127_63(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    setPixelInBuffer(fb, 127, 63, 1);
    return true;
}

static bool testPixelCenter(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    setPixelInBuffer(fb, 64, 32, 1);  // Center of 128×64
    return true;
}

static bool testHorizontalLines(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    // Draw horizontal lines every 8 pixels (8 lines total)
    for (uint8_t y = 0; y < 64; y += 8) {
        drawHLineInBuffer(fb, 0, y, 128, 1);
    }
    return true;
}

static bool testVerticalLines(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    // Draw vertical lines every 8 pixels (16 lines total)
    for (uint8_t x = 0; x < 128; x += 8) {
        drawVLineInBuffer(fb, x, 0, 64, 1);
    }
    return true;
}

static bool testGrid(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    // Horizontal lines every 8 pixels
    for (uint8_t y = 0; y < 64; y += 8) {
        drawHLineInBuffer(fb, 0, y, 128, 1);
    }
    // Vertical lines every 8 pixels
    for (uint8_t x = 0; x < 128; x += 8) {
        drawVLineInBuffer(fb, x, 0, 64, 1);
    }
    return true;
}

static bool testCheckerboard(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    // 2×2 checkerboard pattern
    for (uint8_t y = 0; y < 64; y++) {
        for (uint8_t x = 0; x < 128; x++) {
            // Pixel ON if (x/2 + y/2) is even
            uint8_t checker = ((x / 2) + (y / 2)) & 1;
            setPixelInBuffer(fb, x, y, checker);
        }
    }
    return true;
}

static bool testGradient(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    
    // Create 5-level gradient from top (bright) to bottom (dark)
    // Simulates Doom dithering: 5 brightness levels
    uint8_t thresholds[5] = { 204, 153, 102, 51, 0 };  // Top to bottom
    
    for (uint8_t y = 0; y < 64; y++) {
        uint8_t intensity = (y * 5) / 64;  // Map y to intensity level
        if (intensity > 4) intensity = 4;
        
        for (uint8_t x = 0; x < 128; x++) {
            // Dither based on intensity and position
            uint8_t dither_val = (x + y * 128) & 0xFF;
            uint8_t pixel = (dither_val < thresholds[intensity]) ? 1 : 0;
            setPixelInBuffer(fb, x, y, pixel);
        }
    }
    return true;
}

static bool testCompleteScene(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    
    // WALLS (thick vertical lines on edges)
    // Left wall
    drawRectInBuffer(fb, 0, 0, 16, 64, 1);
    // Right wall
    drawRectInBuffer(fb, 112, 0, 16, 64, 1);
    
    // HUD (horizontal line at bottom)
    drawHLineInBuffer(fb, 0, 56, 128, 1);
    // HUD background (8-pixel bar)
    for (uint8_t y = 57; y < 64; y++) {
        drawHLineInBuffer(fb, 0, y, 128, 1);
    }
    
    // CROSSHAIR (center marker - simple cross)
    drawHLineInBuffer(fb, 60, 32, 8, 1);   // Horizontal bar (64±4)
    drawVLineInBuffer(fb, 64, 28, 8, 1);   // Vertical bar (32±4)
    
    // HEALTH INDICATOR (left corner of HUD)
    drawRectInBuffer(fb, 2, 58, 12, 4, 1);
    
    // AMMO INDICATOR (right corner of HUD)
    drawRectInBuffer(fb, 114, 58, 12, 4, 1);
    
    return true;
}

static bool testScrollingPattern(uint8_t* fb) {
    fillFramebuffer(fb, 0x00);
    
    // Vertical bars that scroll
    static uint32_t frame = 0;
    uint8_t offset = (frame / 6) % 8;  // Change every 6 frames (slow animation)
    frame++;
    
    for (uint8_t y = 0; y < 64; y++) {
        for (uint8_t x = 0; x < 128; x++) {
            // Vertical stripe pattern that shifts
            uint8_t stripe = ((x + offset) / 4) & 1;
            setPixelInBuffer(fb, x, y, stripe);
        }
    }
    
    return true;
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

bool runTest(TestPattern pattern) {
    uint8_t* framebuffer = DoomGraphics::getFramebuffer();
    if (!framebuffer) {
        Serial.println(F("[VALIDATION] ERROR: Framebuffer NULL"));
        return false;
    }
    
    bool result = false;
    
    Serial.printf("[VALIDATION] Running test: %s\r\n", getTestDescription(pattern));
    Serial.printf("[VALIDATION] Expected: %s\r\n", getExpectedResult(pattern));
    
    switch (pattern) {
        case TEST_PATTERN_ALL_BLACK:
            result = testAllBlack(framebuffer);
            break;
        case TEST_PATTERN_ALL_WHITE:
            result = testAllWhite(framebuffer);
            break;
        case TEST_PATTERN_PIXEL_0_0:
            result = testPixel_0_0(framebuffer);
            break;
        case TEST_PATTERN_PIXEL_127_0:
            result = testPixel_127_0(framebuffer);
            break;
        case TEST_PATTERN_PIXEL_0_63:
            result = testPixel_0_63(framebuffer);
            break;
        case TEST_PATTERN_PIXEL_127_63:
            result = testPixel_127_63(framebuffer);
            break;
        case TEST_PATTERN_CENTER:
            result = testPixelCenter(framebuffer);
            break;
        case TEST_PATTERN_HORIZONTAL_LINES:
            result = testHorizontalLines(framebuffer);
            break;
        case TEST_PATTERN_VERTICAL_LINES:
            result = testVerticalLines(framebuffer);
            break;
        case TEST_PATTERN_GRID:
            result = testGrid(framebuffer);
            break;
        case TEST_PATTERN_CHECKERBOARD:
            result = testCheckerboard(framebuffer);
            break;
        case TEST_PATTERN_GRADIENT:
            result = testGradient(framebuffer);
            break;
        case TEST_PATTERN_COMPLETE_SCENE:
            result = testCompleteScene(framebuffer);
            break;
        case TEST_PATTERN_SCROLLING_PATTERN:
            result = testScrollingPattern(framebuffer);
            break;
        default:
            Serial.printf("[VALIDATION] ERROR: Unknown pattern %d\r\n", pattern);
            return false;
    }
    
    if (result) {
        Serial.println(F("[VALIDATION] ✓ Test generated successfully"));
    } else {
        Serial.println(F("[VALIDATION] ✗ Test generation FAILED"));
    }
    
    return result;
}

}  // namespace DoomValidationTest
