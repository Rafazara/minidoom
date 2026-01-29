# MINI DOOM RENDER VALIDATION - QUICK START CARD

## TL;DR - Do This Now

### 1️⃣ Apply 3 Code Fixes (5 minutes)

**Fix A:** [src/doom_graphics.cpp](src/doom_graphics.cpp) line 74 & 88
```diff
- uint8_t bit_index = x % 8;
+ uint8_t bit_index = 7 - (x % 8);
```

**Fix B:** [src/doom_world_renderer.cpp](src/doom_world_renderer.cpp) line 100
```diff
- uint8_t bit = y & 7;
+ uint8_t bit = 7 - (y & 7);
```

**Fix C:** [platformio.ini](platformio.ini)
```diff
  build_flags = 
+     -D MINI_DOOM_DEBUG
+     -D VALIDATION_MODE
```

### 2️⃣ Add Test Command Handler to main.cpp (2 minutes)

Add after includes:
```cpp
#include "doom_validation_test.h"

void handleValidationCommands() {
    #if defined(MINI_DOOM_DEBUG) || defined(VALIDATION_MODE)
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        int testIndex = -1;
        switch (cmd) {
            case '0': testIndex = 0; break;
            case '1': testIndex = 1; break;
            case '2': testIndex = 2; break;
            case '3': testIndex = 3; break;
            case '4': testIndex = 4; break;
            case '5': testIndex = 5; break;
            case '6': testIndex = 6; break;
            case '7': testIndex = 7; break;
            case '8': testIndex = 8; break;
            case '9': testIndex = 9; break;
            case 'a': testIndex = 10; break;
            case 'b': testIndex = 11; break;
            case 'c': testIndex = 12; break;
            case 'd': testIndex = 13; break;
        }
        if (testIndex >= 0) {
            DoomValidationTest::runTest((DoomValidationTest::TestPattern)testIndex);
            DoomGraphics::blitBufferToOLED();
        }
    }
    #endif
}
```

Add to MODE_DOOM loop:
```cpp
handleValidationCommands();  // NEW
DoomAdapter::renderFrame();
DoomGraphics::blitBufferToOLED();
```

### 3️⃣ Compile & Upload (2 minutes)
```bash
pio run -t upload
```

### 4️⃣ Open Serial Monitor (1 minute)
```bash
pio device monitor -b 115200
```

### 5️⃣ Run Tests (5 minutes)
Send commands and observe OLED:
```
Send: 0  → Should see BLACK screen
Send: 1  → Should see WHITE screen
Send: 2  → Should see pixel at TOP-LEFT corner only
Send: 3  → Should see pixel at TOP-RIGHT corner only
Send: 7  → Should see horizontal lines (8 stripes)
Send: a  → Should see checkerboard pattern
Send: c  → Should see complete scene (walls, HUD, crosshair)
```

---

## If It Works ✅

OLED shows correct patterns → Display pipeline is valid → Enable gameplay

---

## If Something's Wrong ❌

| OLED Shows | Likely Issue | Fix |
|-----------|------------|-----|
| Nothing / Blank | I2C disconnected | Check wiring: SDA=GPIO21, SCL=GPIO22 |
| Tests 0-1 work, but pixels in wrong position | Bit order still wrong | Verify Fix A and B applied correctly |
| Pixels visible but offset | Framebuffer indexing | Check byte formula: `(y/8)*128+x` |
| Series fails to connect | Baud rate mismatch | Ensure `monitor_speed = 115200` in platformio.ini |

---

## Test Pattern Meanings

| Command | Expected | Meaning if Correct |
|---------|----------|-------------------|
| `0` | All black | Display connected and can turn off pixels |
| `1` | All white | Display can turn on all pixels |
| `2-5` | Pixels at corners | Coordinate mapping (X, Y) correct |
| `6` | Pixel at center | Frame buffer read/write working |
| `7-9` | Lines and grids | Raster rendering working |
| `a-b` | Patterns | Dithering and intensity working |
| `c` | Full scene | All rendering layers working together |

---

## Success Criteria

✅ Commands 0-1 → Connectivity OK
✅ Commands 2-6 → Coordinate mapping OK
✅ Commands 7-9 → Rendering OK
✅ Commands a-b → Patterns OK
✅ Command c → Complete pipeline OK
✅ Boot game → Title screen appears
✅ Press fire → PLAYING state renders
✅ Gameplay → Works end-to-end

**All checks pass → Pipeline is 100% correct**

---

## File Locations

| File | Purpose | Change |
|------|---------|--------|
| src/doom_graphics.cpp | Framebuffer ops | Line 74, 88: Add `7 - ` before bit index |
| src/doom_world_renderer.cpp | Raycasting | Line 100: Add `7 - ` before bit calculation |
| platformio.ini | Build config | Add `-D MINI_DOOM_DEBUG -D VALIDATION_MODE` |
| src/main.cpp | Game loop | Add handleValidationCommands() + call in loop |

---

## Key Insights

**Why the bit-order fix?**
- Adafruit SSD1306 uses MSB-first (bit 7 = left pixel)
- Current code uses LSB-first (bit 0 = left pixel)
- Mismatch causes left-right mirroring

**Why the validation mode?**
- Game needs fire button to transition from TITLE to PLAYING
- Fire button might not be connected or working
- Validation mode auto-transitions after 3 seconds for testing

**Why the test patterns?**
- Allows testing display pipeline without game logic
- Each pattern tests one part of the system
- All tests pass → Pipeline is correct

---

## Performance Impact

After fixes: **NO CHANGE** (bit reversal is same speed as bit calculation)
- Game update: Still ~35-40ms
- Rendering: Still ~15-20ms
- OLED blit: Still ~2-3ms
- Total frame: Still 50ms @ 20 FPS

---

## Verification Checklist

- [ ] Fix A applied (doom_graphics.cpp lines 74, 88)
- [ ] Fix B applied (doom_world_renderer.cpp line 100)
- [ ] Fix C applied (platformio.ini build flags)
- [ ] handleValidationCommands() added to main.cpp
- [ ] Function called in MODE_DOOM loop
- [ ] Compiles without errors
- [ ] Uploads successfully
- [ ] Serial monitor shows initialization
- [ ] Tests 0-1 display correctly on OLED
- [ ] Tests 2-6 show pixels at correct corners
- [ ] Tests 7-9 show aligned lines
- [ ] Tests a-c show patterns and scene
- [ ] Title screen appears on boot
- [ ] Fire button transitions to PLAYING
- [ ] Game renders with walls and sprites

---

## Pro Tips

1. Keep serial monitor open during testing for diagnostics
2. If a test fails, try test 0 and 1 first to verify display still works
3. Test patterns don't clear each other, so send '0' between tests to reset
4. Once all tests pass, remove VALIDATION_MODE to clean up binary
5. Run complete test sequence before enabling full gameplay

---

## Common Mistakes

❌ Don't forget the `-D` prefix in build flags
❌ Don't change GPIO pins without checking wiring
❌ Don't skip the bit-order fix (causes display corruption)
❌ Don't assume serial commands work without `-D MINI_DOOM_DEBUG`
❌ Don't test gameplay until validation tests pass

---

## Reference: OLED Memory Layout

```
Framebuffer (1024 bytes):
┌─────────────────────────────────┐
│ Byte 0-127:   PAGE 0 (pixels 0-7)   │  Y=0
│ Byte 128-255: PAGE 1 (pixels 8-15)  │  Y=8
│ Byte 256-383: PAGE 2 (pixels 16-23) │  Y=16
│ ...                                 │
│ Byte 896-1023: PAGE 7 (pixels 56-63)│  Y=56
└─────────────────────────────────┘

Bit Order (MSB = leftmost pixel):
Byte value: 0b10000000 = pixel at leftmost position (bit 7)
Byte value: 0b00000001 = pixel at rightmost position (bit 0)

Formula to set pixel (x, y):
  byte_index = (y / 8) * 128 + x
  bit_index = 7 - (y % 8)  ← THIS IS THE FIX
  buffer[byte_index] |= (1 << bit_index)
```

---

## Estimated Time

- Apply fixes: **5 minutes**
- Add test handler: **2 minutes**
- Compile & upload: **2 minutes**
- Serial monitor: **1 minute**
- Run tests: **5 minutes**
- Total: **~15 minutes**

---

**Status:** Ready for implementation  
**Created:** Phase 6 - Validation Test Suite  
**Next:** Follow [VALIDATION_CHECKLIST.md](VALIDATION_CHECKLIST.md) for detailed steps  

