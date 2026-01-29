# RENDER PIPELINE VISUAL VALIDATION TEST SUITE

## Overview

This document describes how to validate the complete render pipeline (framebuffer → OLED display) using deterministic test patterns, without relying on complex game logic.

**Goal:** Confirm 100% display correctness before enabling gameplay.

---

## Test Strategy (6 Stages)

### Stage 1: Display Connectivity (Sanity Check)
Test basic I2C communication and display responsiveness.

| Test | Pattern | Expected OLED Output | Pass Criteria |
|------|---------|----------------------|---------------|
| **Blank Screen** | All pixels OFF | Completely black/dark display | No visible artifacts |
| **Solid White** | All pixels ON | Completely white/bright display | Uniform brightness |

**Diagnostics:** If both tests fail, OLED not responding (I2C issue). If mixed results, possible power issue.

---

### Stage 2: Coordinate Mapping (Display Orientation & Bit Order)
Test individual pixels at corners and center to verify framebuffer bit order and display orientation.

| Test | Pattern | Expected Location | Pass Criteria |
|------|---------|-------------------|---------------|
| **Top-Left** | Pixel at (0,0) | Single bright pixel at **TOP-LEFT corner** | Pixel visible at correct corner |
| **Top-Right** | Pixel at (127,0) | Single bright pixel at **TOP-RIGHT corner** | Pixel visible at correct corner |
| **Bottom-Left** | Pixel at (0,63) | Single bright pixel at **BOTTOM-LEFT corner** | Pixel visible at correct corner |
| **Bottom-Right** | Pixel at (127,63) | Single bright pixel at **BOTTOM-RIGHT corner** | Pixel visible at correct corner |
| **Center** | Pixel at (64,32) | Single bright pixel at **CENTER of screen** | Pixel visible at center |

**Diagnostics:**
- If pixel appears in **wrong corner:** Framebuffer bit order is inverted (apply bit-reversal fix)
- If pixel appears **offset from corner:** Display orientation wrong (check SSD1306 initialization)
- If **no pixel visible:** Pixel rendering function broken

---

### Stage 3: Line Rendering (Raster Correctness)
Test horizontal and vertical lines to verify continuous pixel rendering.

| Test | Pattern | Expected Output | Pass Criteria |
|------|---------|-----------------|---------------|
| **Horizontal Lines** | Lines every 8 pixels (8 total) | 8 evenly-spaced horizontal stripes | All stripes equally spaced, solid |
| **Vertical Lines** | Lines every 8 pixels (16 total) | 16 evenly-spaced vertical stripes | All stripes equally spaced, solid |
| **Grid** | Combined H+V lines | Regular 8×8 grid pattern | Grid squares all same size, aligned |

**Diagnostics:**
- If stripes are **uneven or gaps:** Coordinate wrapping or boundary error
- If stripes are **offset:** Byte indexing calculation wrong
- If stripes are **missing entirely:** Rendering loop not executing

---

### Stage 4: Dithering & Intensity (Tone Rendering)
Test multi-level grayscale to verify dithering algorithm.

| Test | Pattern | Expected Output | Pass Criteria |
|------|---------|-----------------|---------------|
| **Checkerboard** | 2×2 alternating pixels | Fine black/white alternating pattern | Clean checkerboard, no artifacts |
| **Gradient** | 5-level brightness gradient (top to bottom) | Smooth fade from light (top) to dark (bottom) in ~5 distinct bands | Bands visible, no banding artifacts |

**Diagnostics:**
- If checkerboard is **blurry or jagged:** Pixel addressing wrong
- If gradient is **uniform or missing:** Dithering not applied
- If gradient is **banded too coarsely:** Intensity levels miscalculated

---

### Stage 5: Complete Scene (Integrated Rendering)
Test realistic scene with walls, HUD, and crosshair.

**Layout:**
```
┌──────────────────────────────────────────┐
│ ╔════════════════════════════════════╗   │
│ ║                                    ║   │
│ ║            + (crosshair)           ║   │
│ ║                                    ║   │
│ ║      GAME WORLD (center area)      ║   │
│ ║                                    ║   │
│ ║                                    ║   │
│ ╠════════════════════════════════════╣   │ ← HUD bar (bottom 8 pixels)
│ ║ [H]                            [A] ║   │ ← Health/Ammo indicators
│ ╚════════════════════════════════════╝   │
└──────────────────────────────────────────┘
```

**Elements:**
- **Left Wall:** 16-pixel wide rectangle on left edge
- **Right Wall:** 16-pixel wide rectangle on right edge
- **HUD Bar:** Horizontal line + 8-pixel filled bar at bottom
- **Crosshair:** 8×8 cross at (64, 32) center
- **Health Box:** Small rectangle at bottom-left
- **Ammo Box:** Small rectangle at bottom-right

**Pass Criteria:**
- All walls visible and properly aligned
- HUD bar at correct height
- Crosshair centered on screen
- Health/Ammo boxes in correct corners
- No pixel corruption or overlap artifacts

---

### Stage 6: Animation Test (Frame Coherence)
Test that framebuffer updates consistently across multiple frames.

**Pattern:** Vertical bars scroll from left to right (or pulse)

**Pass Criteria:**
- Pattern animates smoothly
- No flickering or tearing
- All frames render complete
- OLED updates at expected rate

---

## How to Run Tests

### Method 1: Serial Monitor Commands (Interactive)

Add to `main.cpp` or create a debug serial listener:

```cpp
void processSerialCommands() {
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch (cmd) {
            case '0': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_ALL_BLACK); break;
            case '1': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_ALL_WHITE); break;
            case '2': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_PIXEL_0_0); break;
            case '3': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_PIXEL_127_0); break;
            case '4': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_PIXEL_0_63); break;
            case '5': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_PIXEL_127_63); break;
            case '6': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_CENTER); break;
            case '7': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_HORIZONTAL_LINES); break;
            case '8': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_VERTICAL_LINES); break;
            case '9': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_GRID); break;
            case 'a': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_CHECKERBOARD); break;
            case 'b': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_GRADIENT); break;
            case 'c': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_COMPLETE_SCENE); break;
            case 'd': DoomValidationTest::runTest(DoomValidationTest::TEST_PATTERN_SCROLLING_PATTERN); break;
        }
        
        // After running test, blit to OLED
        DoomGraphics::blitBufferToOLED();
    }
}
```

Then in `main.cpp::loop()`:
```cpp
processSerialCommands();
```

**Usage in Serial Monitor:**
```
Send: 0   → Blank screen
Send: 1   → Solid white
Send: 2   → Top-left pixel
Send: 6   → Center pixel
Send: 7   → Horizontal lines
Send: 9   → Grid
Send: a   → Checkerboard
Send: b   → Gradient
Send: c   → Complete scene
Send: d   → Scrolling pattern
```

### Method 2: Boot-time Test Sequence

Add to `DoomAdapter::initialize()`:

```cpp
#ifdef VALIDATION_MODE
    // Run validation test sequence
    Serial.println(F("[VALIDATION] Starting test sequence..."));
    delay(1000);
    
    for (int i = 0; i < 14; i++) {
        DoomValidationTest::runTest((DoomValidationTest::TestPattern)i);
        DoomGraphics::blitBufferToOLED();
        delay(3000);  // 3 seconds per test
        Serial.printf("[VALIDATION] Test %d complete, press any key to continue\r\n", i);
        while (!Serial.available()) { delay(10); }
        Serial.read();  // Consume byte
    }
    
    Serial.println(F("[VALIDATION] Sequence complete!"));
#endif
```

Then compile with: `-D VALIDATION_MODE` in `platformio.ini`

---

## Acceptance Criteria Checklist

### ✅ Stage 1: Display Connectivity
- [ ] Test 0 (ALL_BLACK): Completely dark screen
- [ ] Test 1 (ALL_WHITE): Completely bright screen

### ✅ Stage 2: Coordinate Mapping
- [ ] Test 2 (PIXEL_0_0): Single pixel at TOP-LEFT only
- [ ] Test 3 (PIXEL_127_0): Single pixel at TOP-RIGHT only
- [ ] Test 4 (PIXEL_0_63): Single pixel at BOTTOM-LEFT only
- [ ] Test 5 (PIXEL_127_63): Single pixel at BOTTOM-RIGHT only
- [ ] Test 6 (CENTER): Single pixel at CENTER only

### ✅ Stage 3: Line Rendering
- [ ] Test 7 (HORIZONTAL_LINES): 8 evenly-spaced horizontal stripes
- [ ] Test 8 (VERTICAL_LINES): 16 evenly-spaced vertical stripes
- [ ] Test 9 (GRID): Regular 8×8 grid, all squares same size

### ✅ Stage 4: Dithering & Intensity
- [ ] Test 10 (CHECKERBOARD): Fine checkerboard pattern, no gaps
- [ ] Test 11 (GRADIENT): Smooth 5-level fade from light to dark

### ✅ Stage 5: Complete Scene
- [ ] Test 12 (COMPLETE_SCENE): All walls visible
- [ ] Test 12: Crosshair centered
- [ ] Test 12: HUD bar at bottom
- [ ] Test 12: Health/Ammo boxes in correct corners
- [ ] Test 12: No pixel corruption or overlaps

### ✅ Stage 6: Animation
- [ ] Test 13 (SCROLLING_PATTERN): Smooth animation, no tearing/flicker

---

## Diagnostics: If Tests Fail

### All tests black or no response
**Probable cause:** OLED not powered or I2C not responding  
**Check:**
1. VCC (3.3V) connected to OLED pin 1
2. GND connected to OLED pin 2
3. SDA (GPIO 21) connected to OLED pin 3
4. SCL (GPIO 22) connected to OLED pin 4
5. Run MODE_I2C_SCAN to verify address (should be 0x3C or 0x3D)

### Stage 1 passes, but corner pixels in wrong positions (Stage 2)
**Probable cause:** Framebuffer bit order incorrect  
**Solution:** Apply bit reversal fix in doom_graphics.cpp

**Before fix:**
```cpp
uint8_t bit_index = x % 8;  // LSB-first
```

**After fix:**
```cpp
uint8_t bit_index = 7 - (x % 8);  // MSB-first (matches Adafruit drawBitmap)
```

### Lines appear broken or offset (Stage 3)
**Probable cause:** Framebuffer byte indexing wrong  
**Check:** Ensure formula is: `(y/8) * 128 + x`

### Gradient missing or banded too coarsely (Stage 4)
**Probable cause:** Dithering algorithm not applied  
**Check:** Test generation is using simple thresholding. For real Doom, apply proper Bayer dithering.

### Complete scene corrupted (Stage 5)
**Probable cause:** Multiple rendering layers interfering  
**Check:** Verify overlay layers don't clear previous content

### Animation tearing or flicker (Stage 6)
**Probable cause:** Display updates not synchronized  
**Check:** Ensure `blitBufferToOLED()` called every loop iteration

---

## Reference: Test Pattern Byte Layouts

### Test: Pixel at (64, 32) - Center

**Framebuffer calculation:**
- X = 64, Y = 32
- Byte index = (32 / 8) * 128 + 64 = 4 * 128 + 64 = 576
- Bit index = 7 - (32 % 8) = 7 - 0 = 7

**Framebuffer byte 576:** Set bit 7 (MSB)
- Value: `0b10000000` = `0x80`

**Visual:** Single bright pixel at exact center of display

---

### Test: Horizontal Lines (Every 8 pixels)

**Lines at Y = 0, 8, 16, 24, 32, 40, 48, 56**

Each line fills all X pixels (0-127) at that Y:

| Y | Byte Range | Pattern |
|---|------------|---------|
| 0-7 | [0-127] | All 0xFF (full row) |
| 8-15 | [128-255] | All 0xFF (full row) |
| 16-23 | [256-383] | All 0xFF (full row) |
| ... | ... | ... |
| 56-63 | [896-1023] | All 0xFF (full row) |

**Visual:** 8 horizontal stripes with 8-pixel gaps

---

### Test: Checkerboard (2×2 pattern)

**Formula per pixel:**
```cpp
checker = ((x / 2) + (y / 2)) & 1;
```

Creates alternating on/off pixels:
```
Square (x=0-3, y=0-3):
  ○●○●
  ●○●○
  ○●○●
  ●○●○
```

**Visual:** Fine checkerboard pattern across entire display

---

## Professional Validation Flow

```
1. CONNECTIVITY CHECK
   ├─ Test 0 & 1 pass?
   │  No → I2C issue, check wiring
   │  Yes → Continue
   ├─→ 2. COORDINATE MAPPING
       ├─ Tests 2-6 all pass?
       │  No → Bit order or orientation wrong
       │  Yes → Continue
       ├─→ 3. LINE RENDERING
           ├─ Tests 7-9 all pass?
           │  No → Byte indexing or bounds checking wrong
           │  Yes → Continue
           ├─→ 4. DITHERING & INTENSITY
               ├─ Tests 10-11 all pass?
               │  No → Dithering algorithm wrong
               │  Yes → Continue
               ├─→ 5. COMPLETE SCENE
                   ├─ Test 12 all elements visible?
                   │  No → Overlay layer corruption
                   │  Yes → Continue
                   ├─→ 6. ANIMATION
                       ├─ Test 13 smooth & no tearing?
                       │  No → Frame sync issue
                       │  Yes → ✅ PIPELINE VALIDATED
```

---

## Summary

This test suite provides **deterministic, repeatable validation** of the entire render pipeline without any game logic.

**Time to completion:** ~45 seconds (14 tests × 3 seconds each)

**Outcome:** 100% confidence in display correctness before enabling gameplay.

**Next step:** Once all tests pass, enable game logic and verify in-game rendering matches validation patterns.

