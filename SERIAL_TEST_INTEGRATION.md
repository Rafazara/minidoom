# SERIAL TEST COMMAND INTEGRATION

## Quick Integration (Copy-Paste Ready)

Add this to your `src/main.cpp` in the `MODE_DOOM` section:

```cpp
// At the top of main.cpp, after includes
#include "doom_validation_test.h"

// ============================================================================
// VALIDATION TEST HANDLER (Add to main loop)
// ============================================================================

void handleValidationCommands() {
    #if defined(MINI_DOOM_DEBUG) || defined(VALIDATION_MODE)
    
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        Serial.printf("\r\n[TEST] Command received: '%c'\r\n", cmd);
        
        int testIndex = -1;
        
        // Map serial commands to test patterns
        switch (cmd) {
            case '0': testIndex = DoomValidationTest::TEST_PATTERN_ALL_BLACK; 
                      Serial.println("[TEST] Running: All Black"); break;
            case '1': testIndex = DoomValidationTest::TEST_PATTERN_ALL_WHITE; 
                      Serial.println("[TEST] Running: All White"); break;
            case '2': testIndex = DoomValidationTest::TEST_PATTERN_PIXEL_0_0; 
                      Serial.println("[TEST] Running: Pixel (0,0) - Top-Left"); break;
            case '3': testIndex = DoomValidationTest::TEST_PATTERN_PIXEL_127_0; 
                      Serial.println("[TEST] Running: Pixel (127,0) - Top-Right"); break;
            case '4': testIndex = DoomValidationTest::TEST_PATTERN_PIXEL_0_63; 
                      Serial.println("[TEST] Running: Pixel (0,63) - Bottom-Left"); break;
            case '5': testIndex = DoomValidationTest::TEST_PATTERN_PIXEL_127_63; 
                      Serial.println("[TEST] Running: Pixel (127,63) - Bottom-Right"); break;
            case '6': testIndex = DoomValidationTest::TEST_PATTERN_CENTER; 
                      Serial.println("[TEST] Running: Center Pixel (64,32)"); break;
            case '7': testIndex = DoomValidationTest::TEST_PATTERN_HORIZONTAL_LINES; 
                      Serial.println("[TEST] Running: Horizontal Lines"); break;
            case '8': testIndex = DoomValidationTest::TEST_PATTERN_VERTICAL_LINES; 
                      Serial.println("[TEST] Running: Vertical Lines"); break;
            case '9': testIndex = DoomValidationTest::TEST_PATTERN_GRID; 
                      Serial.println("[TEST] Running: Grid Pattern"); break;
            case 'a': testIndex = DoomValidationTest::TEST_PATTERN_CHECKERBOARD; 
                      Serial.println("[TEST] Running: Checkerboard"); break;
            case 'b': testIndex = DoomValidationTest::TEST_PATTERN_GRADIENT; 
                      Serial.println("[TEST] Running: Gradient"); break;
            case 'c': testIndex = DoomValidationTest::TEST_PATTERN_COMPLETE_SCENE; 
                      Serial.println("[TEST] Running: Complete Scene"); break;
            case 'd': testIndex = DoomValidationTest::TEST_PATTERN_SCROLLING_PATTERN; 
                      Serial.println("[TEST] Running: Scrolling Pattern"); break;
            
            case 'h':
            case '?':
                Serial.println("\r\n[TEST] Validation Commands:");
                Serial.println("  0 - All Black (blank screen)");
                Serial.println("  1 - All White (solid screen)");
                Serial.println("  2 - Pixel at (0,0) - Top-Left corner");
                Serial.println("  3 - Pixel at (127,0) - Top-Right corner");
                Serial.println("  4 - Pixel at (0,63) - Bottom-Left corner");
                Serial.println("  5 - Pixel at (127,63) - Bottom-Right corner");
                Serial.println("  6 - Pixel at (64,32) - Center");
                Serial.println("  7 - Horizontal Lines");
                Serial.println("  8 - Vertical Lines");
                Serial.println("  9 - Grid Pattern");
                Serial.println("  a - Checkerboard");
                Serial.println("  b - Gradient (5-level intensity)");
                Serial.println("  c - Complete Scene (walls + HUD + crosshair)");
                Serial.println("  d - Scrolling Pattern");
                Serial.println("  h - This help message");
                Serial.println();
                return;
            
            default:
                Serial.printf("[TEST] Unknown command '%c'. Type 'h' for help.\r\n", cmd);
                return;
        }
        
        if (testIndex >= 0) {
            // Run test pattern
            DoomValidationTest::runTest((DoomValidationTest::TestPattern)testIndex);
            
            // Display result on OLED
            DoomGraphics::blitBufferToOLED();
            
            // Print expected result
            Serial.printf("[TEST] Expected result: %s\r\n", 
                          DoomValidationTest::getTestDescription((DoomValidationTest::TestPattern)testIndex));
            Serial.println("[TEST] Look at OLED and verify pattern matches...");
        }
    }
    
    #endif
}
```

### In `main.cpp::loop()` for MODE_DOOM:

```cpp
void loop() {
    if (g_currentMode == MODE_DOOM) {
        
        // *** ADD THIS LINE ***
        handleValidationCommands();  // Process serial test commands
        
        // Existing game loop code
        DoomAdapter::renderFrame();
        DoomGraphics::blitBufferToOLED();
    }
}
```

---

## Enable Debug Mode in platformio.ini

### For Development/Testing:

```ini
[env:esp32]
platform = espressif32
board = esp32devkit
framework = arduino
upload_speed = 921600
monitor_speed = 115200

; Enable MINI_DOOM_DEBUG for test command support
build_flags = 
    -D MINI_DOOM_DEBUG
    -D DEBUG_MODE
    ; Optionally add:
    ; -D VALIDATION_MODE  (for automatic test sequence on boot)

lib_deps =
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
```

### For Production (Remove Debug):

```ini
[env:esp32-release]
platform = espressif32
board = esp32devkit
framework = arduino
upload_speed = 921600

; No debug flags - cleaner binary, faster execution
build_flags =
    -Os
    -D NDEBUG

lib_deps =
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
```

---

## Usage Example: Step-by-Step Validation

### Step 1: Compile & Upload

```bash
pio run -t upload
```

### Step 2: Open Serial Monitor

```bash
pio device monitor -b 115200
```

### Step 3: Send Test Commands

```
# Test 1: Check display connectivity
> 0
[TEST] Command received: '0'
[TEST] Running: All Black
[TEST] Expected result: All pixels OFF (blank black screen)
[TEST] Look at OLED and verify pattern matches...

# Verify OLED is completely black

# Test 2: Check connectivity (opposite)
> 1
[TEST] Command received: '1'
[TEST] Running: All White
[TEST] Expected result: All pixels ON (solid white screen)
[TEST] Look at OLED and verify pattern matches...

# Verify OLED is completely white/bright

# Test 3: Check corner pixels
> 2
[TEST] Command received: '2'
[TEST] Running: Pixel (0,0) - Top-Left
[TEST] Expected result: Single pixel at TOP-LEFT corner
[TEST] Look at OLED and verify pattern matches...

# Verify single bright pixel appears at TOP-LEFT only

> 3
[TEST] Command received: '3'
[TEST] Running: Pixel (127,0) - Top-Right
[TEST] Expected result: Single pixel at TOP-RIGHT corner
[TEST] Look at OLED and verify pattern matches...

# Verify single bright pixel appears at TOP-RIGHT only

# Test 4: Check line rendering
> 7
[TEST] Command received: '7'
[TEST] Running: Horizontal Lines
[TEST] Expected result: 8 evenly-spaced horizontal stripes
[TEST] Look at OLED and verify pattern matches...

# Verify 8 horizontal lines with equal spacing

# Test 5: Check dithering
> a
[TEST] Command received: 'a'
[TEST] Running: Checkerboard
[TEST] Expected result: Fine checkerboard pattern (50% black, 50% white)
[TEST] Look at OLED and verify pattern matches...

# Verify fine checkerboard pattern, not blocky

# Test 6: Complete scene
> c
[TEST] Command received: 'c'
[TEST] Running: Complete Scene
[TEST] Expected result: Walls on edges, crosshair at center, HUD bar at bottom
[TEST] Look at OLED and verify pattern matches...

# Verify all elements present and correctly positioned

# Get help anytime
> h
[TEST] Validation Commands:
  0 - All Black (blank screen)
  1 - All White (solid screen)
  2 - Pixel at (0,0) - Top-Left corner
  ...
```

---

## Automated Validation Sequence

Add to `main.cpp` for boot-time automatic testing:

```cpp
#ifdef VALIDATION_MODE
void runFullValidationSequence() {
    Serial.println("\r\n========================================");
    Serial.println("MINI DOOM - FULL VALIDATION SEQUENCE");
    Serial.println("========================================\r\n");
    
    const int tests[] = {
        DoomValidationTest::TEST_PATTERN_ALL_BLACK,
        DoomValidationTest::TEST_PATTERN_ALL_WHITE,
        DoomValidationTest::TEST_PATTERN_PIXEL_0_0,
        DoomValidationTest::TEST_PATTERN_PIXEL_127_0,
        DoomValidationTest::TEST_PATTERN_PIXEL_0_63,
        DoomValidationTest::TEST_PATTERN_PIXEL_127_63,
        DoomValidationTest::TEST_PATTERN_CENTER,
        DoomValidationTest::TEST_PATTERN_HORIZONTAL_LINES,
        DoomValidationTest::TEST_PATTERN_VERTICAL_LINES,
        DoomValidationTest::TEST_PATTERN_GRID,
        DoomValidationTest::TEST_PATTERN_CHECKERBOARD,
        DoomValidationTest::TEST_PATTERN_GRADIENT,
        DoomValidationTest::TEST_PATTERN_COMPLETE_SCENE,
        DoomValidationTest::TEST_PATTERN_SCROLLING_PATTERN,
    };
    
    const int numTests = sizeof(tests) / sizeof(tests[0]);
    int passedTests = 0;
    unsigned long testStartTime = millis();
    
    for (int i = 0; i < numTests; i++) {
        DoomValidationTest::runTest((DoomValidationTest::TestPattern)tests[i]);
        DoomGraphics::blitBufferToOLED();
        
        Serial.printf("[%02d/%02d] Test: %s\r\n", 
                      i + 1, 
                      numTests, 
                      DoomValidationTest::getTestDescription((DoomValidationTest::TestPattern)tests[i]));
        Serial.println("        ✓ Pattern generated and displayed");
        Serial.println("        ⊙ Waiting 3 seconds (observe OLED, press any key to skip)...\r\n");
        
        // Wait for user input or 3 seconds
        unsigned long startWait = millis();
        while (millis() - startWait < 3000) {
            if (Serial.available()) {
                Serial.read();  // Consume input
                break;
            }
            delay(10);
        }
    }
    
    unsigned long totalTime = millis() - testStartTime;
    Serial.println("\r\n========================================");
    Serial.printf("VALIDATION SEQUENCE COMPLETE\r\n");
    Serial.printf("Total time: %lu seconds\r\n", totalTime / 1000);
    Serial.println("========================================\r\n");
    
    // Return to normal game operation
    delay(2000);
}

// Call this in DoomAdapter::initialize() or main loop startup:
// if (shouldRunValidation) runFullValidationSequence();

#endif
```

---

## Expected Output (Successful Validation)

```
[TEST] Command received: '0'
[TEST] Running: All Black
[TEST] Expected result: All pixels OFF (blank black screen)
[TEST] Look at OLED and verify pattern matches...
OLED: ████████████████████████ (all black)

[TEST] Command received: '1'
[TEST] Running: All White
[TEST] Expected result: All pixels ON (solid white screen)
[TEST] Look at OLED and verify pattern matches...
OLED: ████████████████████████ (all white)

[TEST] Command received: '2'
[TEST] Running: Pixel (0,0) - Top-Left
[TEST] Expected result: Single pixel at TOP-LEFT corner
[TEST] Look at OLED and verify pattern matches...
OLED: ●                        (single pixel at top-left)

[TEST] Command received: '9'
[TEST] Running: Grid Pattern
[TEST] Expected result: Regular 8×8 grid pattern
[TEST] Look at OLED and verify pattern matches...
OLED: ├─┼─┼─┼─┼─┼─┼─┬       (grid of vertical/horizontal lines)

[TEST] Command received: 'c'
[TEST] Running: Complete Scene
[TEST] Expected result: Walls on edges, crosshair at center, HUD bar at bottom
[TEST] Look at OLED and verify pattern matches...
OLED: [Scene with all elements]

✅ ALL TESTS PASSED - DISPLAY PIPELINE VALID
```

---

## Troubleshooting Serial Commands

| Issue | Cause | Solution |
|-------|-------|----------|
| No response to commands | Debug mode not enabled | Add `-D MINI_DOOM_DEBUG` to platformio.ini |
| Commands work but OLED blank | Display not initialized | Verify I2C wiring and initialization code |
| Patterns look wrong | Bit order mismatch | Apply bit-order fix (see RENDER_PIPELINE_AUDIT.md) |
| Serial shows garbage | Baud rate mismatch | Ensure monitor_speed = 115200 in platformio.ini |

