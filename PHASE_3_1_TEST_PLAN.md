# Phase 3.1 - Validation Test Plan

## Unit Tests (Compile-Time)

### ✅ Texture Data Integrity
```cpp
// Verify texture sizes at compile time
static_assert(sizeof(DoomWallTextures::WALL_BRICK) == 32, "BRICK must be 32 bytes");
static_assert(sizeof(DoomWallTextures::WALL_METAL) == 32, "METAL must be 32 bytes");
static_assert(sizeof(DoomWallTextures::WALL_STONE) == 32, "STONE must be 32 bytes");

// Verify constants
static_assert(DoomWallTextures::TEXTURE_SIZE == 16, "Size must be 16");
static_assert(DoomWallTextures::TEXTURE_COUNT == 3, "Count must be 3");
```

### ✅ Memory Layout Validation
```cpp
// Each texture = 16 rows × 2 bytes per row
// Row 0 = bytes [0:2]
// Row 1 = bytes [2:4]
// ...
// Row 15 = bytes [30:32]

static_assert(DoomWallTextures::TEXTURE_BYTES == 32, "Total must be 32");
```

---

## Integration Tests (Runtime)

### Test 1: Basic Texture Sampling
**Objective**: Verify `sampleTexture()` reads correct pixels

**Setup**:
```cpp
const uint8_t* brick = DoomWallTextures::WALL_BRICK;

// First byte of first row: 0xFF (all pixels ON)
uint8_t pixel_0_0 = DoomWallTextures::sampleTexture(brick, 0, 0);  // Should be 1
uint8_t pixel_7_0 = DoomWallTextures::sampleTexture(brick, 7, 0);  // Should be 1

// Position with offset
uint8_t pixel_4_1 = DoomWallTextures::sampleTexture(brick, 4, 1);  // Depends on row 1 data
```

**Expected**: 
- Sampling matches texture bit patterns defined in header
- Wrapping works: (16, 0) should equal (0, 0)
- Wrapping works: (0, 16) should equal (0, 0)

---

### Test 2: Raycasting Integration
**Objective**: Verify textures are applied during raycasting

**Setup**:
```cpp
// Existing framework - no changes needed
DoomWorldRenderer::initialize();

// Render frame with textured walls
DoomWorldRenderer::renderWorld(
    level, player_x, player_y, dir_x, dir_y, plane_x, plane_y, view_height
);

uint8_t* framebuffer = DoomWorldRenderer::getWorldFramebuffer();
const uint8_t* depth = DoomWorldRenderer::getDepthBuffer();
```

**Expected**:
- Framebuffer contains non-zero pixel patterns (not solid color)
- Depth buffer unchanged (still monochromatic distance values)
- Vertical walls show repeating texture pattern
- Horizontal walls show darker version (side darkening +1)

---

### Test 3: Visual Pattern Recognition
**Objective**: On-device verification that textures appear correctly

**Manual Test**:
1. **Compile & Upload**: `pio run -t upload`
2. **Monitor Output**: `pio device monitor`
3. **Look for logs**:
   ```
   [DOOM-WORLD] Wall texture sampling active (Phase 3.1)
   ```
4. **Visual Check on OLED**:
   - ✅ Walls show vertical line patterns (not solid or random noise)
   - ✅ Brick wall: Grid-like pattern with horizontal lines
   - ✅ Close walls: Clear pattern details
   - ✅ Far walls: Pattern dithered but recognizable
   - ✅ Side walls: Noticeably darker than front

---

### Test 4: Performance Benchmark
**Objective**: Confirm 20 FPS is maintained

**Method**:
```
Serial output should show consistent frame timing:
[DOOM-CORE] Frame: 0050ms (20.0 FPS)
[DOOM-CORE] Frame: 0050ms (20.0 FPS)
[DOOM-CORE] Frame: 0050ms (20.0 FPS)
```

**Success Criteria**:
- Frame time ≤ 50ms consistently
- No frame drops below 20 FPS
- No memory growth over multiple frames

---

### Test 5: Sprite Occlusion Still Works
**Objective**: Verify sprites render correctly over textured walls

**Setup**:
```
Shoot wall-adjacent enemy
Expected: Enemy sprite occludes correctly behind wall textures
```

**Validation**:
- Weapon sprite renders on top of walls
- Enemy sprites partially hidden by walls (depth test correct)
- No z-fighting or texture bleeding

---

### Test 6: Memory Footprint
**Objective**: Verify SRAM/Flash usage within bounds

**Method**:
```bash
pio run --verbose 2>&1 | grep -A2 "Memory Usage"
```

**Expected**:
```
RAM:   [=         ]   7.5% (used 24500 bytes from 327680 bytes)
Flash: [===       ]  25.2% (used 329669 bytes from 1310720 bytes)
```

**Success Criteria**:
- SRAM < 30 KB ✅
- SRAM growth < 1 KB vs Phase 2.15 ✅
- Flash increase justified by PROGMEM textures ✅

---

## Regression Tests (Phase Compatibility)

### Wave System
**Test**: Spawn wave, enemies should render with textures
```
Expected: Enemy sprites appear/disappear correctly with wall backdrop
```

### Combat System
**Test**: Fire at wall + enemy
```
Expected:
- Weapon flash persists
- Screen shake works
- Enemy death triggers correctly
```

### HUD Rendering
**Test**: Health/ammo display
```
Expected: HUD elements render on top layer (unchanged)
```

### Feedback Effects
**Test**: Weapon recoil, hit feedback
```
Expected: Effects display without texture artifacts
```

---

## Code Review Checklist

- [x] No new `malloc()` or `new` calls
- [x] All textures in PROGMEM (not SRAM)
- [x] Backward-compatible function signatures
- [x] No changes to public APIs
- [x] No changes to input pipeline
- [x] No changes to sprite renderer (except include fix)
- [x] Depth buffer semantics preserved
- [x] Logging is minimal (≤3 lines at init)
- [x] Compilation successful without warnings
- [x] Includes follow order convention (project before system)

---

## Validation Report Template

```
═══════════════════════════════════════════════════════════════
Phase 3.1 - Wall Textures Validation Report
═══════════════════════════════════════════════════════════════

Date:                    ___________
Tester:                  ___________
Device:                  ESP32 (mini-doom)
OLED:                    SSD1306 128×64 1-bit

COMPILATION
─────────────────────────────────────────────────────────────
✅ Compiles without warnings
   Warnings: 0
   Errors: 0
   Build time: __________ sec

MEMORY
─────────────────────────────────────────────────────────────
✅ SRAM within limit
   Used: __________ KB / 327 KB (________ %)
   Heap free: __________ KB (target: ≥270 KB)

✅ Flash usage acceptable
   Used: __________ KB / 1.3 MB
   PROGMEM textures: 96 bytes

VISUAL QUALITY
─────────────────────────────────────────────────────────────
✅ Textures render correctly
   Wall pattern visible: YES / NO / PARTIAL
   Pattern recognizable as brick/metal: YES / NO / PARTIAL
   Texture tiling continuous: YES / NO / PARTIAL

✅ Depth/shading preserved
   Close walls darker: YES / NO
   Far walls lighter: YES / NO
   Side walls darker than front: YES / NO

PERFORMANCE
─────────────────────────────────────────────────────────────
✅ Frame rate maintained at 20 FPS
   Frame time: __________ ms (target: ≤50ms)
   Frame rate: __________ FPS (target: 20)
   Frame drops: 0 / ___ (count over 30 frames)

COMPATIBILITY
─────────────────────────────────────────────────────────────
✅ Sprites render with correct occlusion
   Weapon sprite: OK / ISSUE / N/A
   Enemy sprites: OK / ISSUE / N/A
   Depth sorting: OK / ISSUE / N/A

✅ HUD elements display correctly
   Health: OK / ISSUE / N/A
   Ammo: OK / ISSUE / N/A
   Wave counter: OK / ISSUE / N/A

✅ Feedback effects work
   Weapon flash: OK / ISSUE / N/A
   Screen shake: OK / ISSUE / N/A
   Enemy kill feedback: OK / ISSUE / N/A

OVERALL RESULT
─────────────────────────────────────────────────────────────
Status: ✅ PASS / ❌ FAIL / ⚠️ CONDITIONAL

Issues Found:
1. _________________________________
2. _________________________________
3. _________________________________

Sign-off:

Tester: ___________________   Date: ___________
```

---

## Notes

- Texture patterns are deliberately simple (high contrast for 1-bit mode)
- Dithering will apply secondary pattern over texture base
- Side darkening creates visual depth through shading difference
- No performance overhead expected (sampling cost negligible vs rendering)
