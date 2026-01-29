# Phase 2.3 Planning - Actual Doom Nano Engine Rendering

**Status**: PLANNING DOCUMENT (NOT YET IMPLEMENTED)  
**Target**: Full Doom Nano raycasting + sprite rendering on ESP32

---

## Overview

Phase 2.3 will replace the test pattern with **actual Doom Nano rendering** by integrating the raycasting engine and sprite system.

### Scope: What Phase 2.3 Includes
- ✅ renderMap() - Raycasting engine (walls, depth)
- ✅ renderEntities() - Sprite rendering (enemies, items)
- ✅ renderGun() - HUD weapon
- ✅ Player state management (position, health, keys)
- ✅ Static scene (single level, no progression)
- ⏳ Dithering pipeline (proven in Phase 2.2)

### Scope: What Phase 2.3 EXCLUDES (Still)
- ❌ Input handling (keyboard/joystick)
- ❌ Audio system (sound effects)
- ❌ Game loop (scene management, state changes)
- ❌ AI/Entity logic (collision, enemy behavior)
- ❌ Menu system or level progression

---

## Technical Architecture

### Current Phase 2.2 Structure

```cpp
doom_adapter.cpp:
  initialize()
    └─ Setup buffers, timing
    
  renderFrame() [non-blocking]
    ├─ Throttle check (50ms)
    ├─ drawDoomTestPattern()
    │  └─ Fill g_doomFramebuffer
    ├─ processFrameWithDithering()
    │  └─ Apply Bayer 2×2
    └─ Return true/false
```

### Phase 2.3 Enhancement

```cpp
doom_adapter.cpp (extended):
  initialize()
    ├─ Setup Doom state (player, level, entities)
    └─ Load level data
    
  renderFrame() [non-blocking]
    ├─ Throttle check (50ms)
    ├─ renderDoomFrame()
    │  ├─ renderMap(level)        ← From display.h
    │  ├─ renderEntities()        ← From display.h
    │  ├─ renderGun(gun_pos)      ← From display.h
    │  └─ Output to g_doomFramebuffer
    ├─ processFrameWithDithering()
    │  └─ Apply Bayer 2×2
    └─ Return true/false
```

---

## Implementation Strategy

### Step 1: Extract Doom Nano Functions (Refactor)

**Source**: `third_party/doom-nano/display.h` (277 lines)

**Functions to Extract**:
```cpp
// Drawing primitives
void drawPixel(int8_t x, int8_t y, bool color, bool raycasterViewport);
void drawVLine(uint8_t x, int8_t start_y, int8_t end_y, uint8_t intensity);
void drawSprite(int8_t x, int8_t y, const uint8_t bitmap[], const uint8_t mask[], 
                int16_t w, int16_t h, uint8_t sprite, double distance);
void drawByte(uint8_t x, uint8_t y, uint8_t b);

// Rendering
void renderMap(const uint8_t level[], double view_height);
void renderEntities(double view_height);
void renderGun(uint8_t gun_pos, double amount_jogging);

// Utility
void drawText(int8_t x, int8_t y, const __FlashStringHelper txt, uint8_t space);
void updateHud();
```

**Challenge**: These functions assume `display.h` scope globals:
- `display_buf` (framebuffer pointer)
- `zbuffer` (depth buffer)
- `gradient` (PROGMEM texture data)
- Adafruit_SSD1306 instance

**Solution**: Create wrapper namespace that provides compatible interface

### Step 2: Adapt to ESP32 / Arduino

**Required Ports**:

1. **Graphics Backend** (done in Phase 2.2)
   ```cpp
   // Old: display_buf = display.getBuffer()
   // New: Uses g_doomFramebuffer (phase 2.2 buffer)
   ```

2. **Type System** (mostly compatible)
   ```cpp
   // doom-nano types:
   struct Coords { double x; double y; }
   struct Player { Coords pos, dir, plane; double velocity; uint8_t health, keys; }
   struct Entity { UID uid; Coords pos; uint8_t state, health, distance, timer; }
   
   // Already compatible with Arduino/ESP32
   ```

3. **Constants** (from doom-nano/constants.h)
   ```cpp
   #define SCREEN_WIDTH 128
   #define SCREEN_HEIGHT 64
   #define RENDER_HEIGHT 56
   #define FRAME_TIME 66.666  // FPS timing
   ```

4. **Memory** (static allocation only)
   ```cpp
   uint8_t zbuffer[ZBUFFER_SIZE];         // Depth buffer
   Entity entity[MAX_ENTITIES];            // Dynamic entities
   StaticEntity static_entity[...];        // Sleeping entities
   Player player;                          // Player state
   ```

### Step 3: Integrate into Phase 2.2 Pipeline

**Modified renderFrame()**:
```cpp
bool renderFrame() {
    if (!initialized) return false;
    
    // Throttle
    if (currentTime - lastFrameTime < ADAPTER_FRAME_DELAY_MS) {
        return false;
    }
    lastFrameTime = currentTime;
    
    // ← NEW PHASE 2.3 ←
    // Clear Doom framebuffer
    memset(g_doomFramebuffer, 0, 1024);
    
    // Set up rendering target
    display_buf = g_doomFramebuffer;
    
    // Render Doom scene
    renderMap(sto_level_1, 0.0);        // Static view height
    renderEntities(0.0);
    renderGun(18, 0.0);
    
    // Phase 2.2 dithering (unchanged)
    processFrameWithDithering();
    
    return true;
}
```

### Step 4: Static Scene Management

**Initial State** (no progression):
```cpp
void initialize() {
    // Fixed player position
    player.pos = {8.0, 4.0};
    player.dir = {1.0, 0.0};
    player.plane = {0.0, -0.66};
    player.health = 100;
    player.keys = 0;
    player.velocity = 0.0;
    
    // Load single level
    initializeLevel(sto_level_1);
    
    // No entities spawned initially
    num_entities = 0;
    num_static_entities = 0;
}
```

**Rendering** (repeating, frozen):
```cpp
// Same frame every time (no input, no movement)
renderMap(sto_level_1, 0.0);
renderEntities(0.0);
renderGun(18, 0.0);
```

---

## Doom Nano Code Analysis

### File Structure to Port

```
third_party/doom-nano/
├── constants.h (50 lines)
│  └─ All #defines, mostly CPU-independent
│
├── types.h (50 lines)
│  └─ struct definitions (UID, Coords, Player, Entity, etc)
│
├── display.h (277 lines) ← PRIMARY TARGET
│  ├─ drawPixel()
│  ├─ drawVLine()
│  ├─ drawSprite()
│  ├─ drawByte()
│  ├─ renderMap()       ← Raycasting engine
│  ├─ renderEntities()  ← Sprite rendering
│  └─ renderGun()       ← HUD
│
├── entities.cpp/h (150 lines)
│  └─ createEntity(), create_player(), etc.
│
├── level.h (level data)
│  └─ sto_level_1[]     ← Static level map
│
└── [NOT NEEDED IN PHASE 2.3]
    ├── input.h         ❌ No input
    ├── sound.h         ❌ No audio
    ├── sprites.h       ⏳ If entity rendering needed
    └── doom-nano.ino   ❌ No main loop
```

### Estimated Lines of Code

```
Functions to port: ~200 lines (from display.h)
Type definitions:  ~50 lines (from types.h)
Constants:         ~20 lines (from constants.h)
Doom adapter mods: ~50 lines (integrate into Phase 2.2)

Total Phase 2.3: ~320 lines of code
```

---

## Validation Plan

### Unit Testing (Implicit)
1. **Compilation**: Zero errors/warnings
2. **Type Safety**: All types compatible
3. **Memory**: Static allocation verified
4. **Rendering**: Each function produces expected output

### Integration Testing
1. **Initialize**: Doom state setup successful
2. **Render Frame**: renderMap() → framebuffer
3. **Sprites**: renderEntities() → framebuffer
4. **HUD**: renderGun() → framebuffer
5. **Dithering**: Bayer 2×2 applied
6. **Display**: OLED shows recognizable Doom scene

### Hardware Testing
- ✅ OLED displays Doom scene (frozen)
- ✅ FPS maintained (≥15 acceptable)
- ✅ Heap stable (≥300KB free)
- ✅ No crashes or artifacts
- ✅ Uptime >10 minutes

---

## Iteration Plan

### Iteration 1: Core Rendering
- [ ] Extract display.h functions
- [ ] Create wrapper namespace
- [ ] Integrate renderMap()
- [ ] Test raycasting output
- [ ] Validate maze rendering

### Iteration 2: Entities & Sprites
- [ ] Add renderEntities()
- [ ] Add renderGun()
- [ ] Spawn test entity
- [ ] Validate sprite rendering
- [ ] Check for visual artifacts

### Iteration 3: HUD & Polish
- [ ] Add health/keys display
- [ ] Add weapon rendering
- [ ] Optimize frame timing
- [ ] Final hardware validation
- [ ] Performance tuning (if needed)

### Iteration 4: Documentation & Release
- [ ] Code comments
- [ ] Architecture documentation
- [ ] Performance metrics
- [ ] Phase 2.3 completion report

---

## Resource Estimates

### Flash Budget Impact

```
Current (Phase 2.2): 305,229 bytes (23.3%)
Phase 2.3 additions:

Functions (~320 lines of C++):
  ├─ Average 50 bytes/function
  ├─ 12-15 functions
  └─ ~600-750 bytes

Constants & static data:
  ├─ Level map (sto_level_1): ~200 bytes
  ├─ Entity arrays: ~500 bytes
  ├─ Gradient/texture data: ~2KB (PROGMEM)
  └─ ~2.7 KB

Estimated Phase 2.3: +3.5-4.0 KB

Projected Total: 309-310 KB (23.5-23.6%)
Safe Margin: 76.4-76.5% remaining ✅ SAFE
```

### RAM Budget Impact

```
Current (Phase 2.2): 24,032 bytes (7.3%)
Phase 2.3 additions:

Game state:
  ├─ Player struct: ~32 bytes
  ├─ Entity array[MAX_ENTITIES]: ~160 bytes
  ├─ zbuffer[ZBUFFER_SIZE]: ~64 bytes
  ├─ Static entities: ~100 bytes
  └─ ~356 bytes

Projected Total: 24,388 bytes (7.4%)
Remaining: 303,292 bytes (92.6%) ✅ EXCELLENT
```

---

## Known Challenges & Solutions

### Challenge 1: Doom Nano Uses Its Own Display Object
**Problem**: `display.h` assumes `Adafruit_SSD1306 display;` global  
**Solution**: Redirect to OLEDUI::getDisplay() or use local reference

### Challenge 2: Framebuffer Pointer
**Problem**: `display_buf = display.getBuffer();` in setupDisplay()  
**Solution**: Set `display_buf = g_doomFramebuffer;` in Phase 2.3 init

### Challenge 3: AVR-Specific Code
**Problem**: Some macros/functions AVR-only  
**Solution**: Skip or provide ESP32 equivalents

### Challenge 4: Performance of Raycasting
**Problem**: Raycasting is CPU-intensive  
**Solution**: Already throttled to 50ms (20 FPS), should handle

### Challenge 5: Texture/Gradient Data
**Problem**: PROGMEM data in doom-nano.ino  
**Solution**: Include PROGMEM arrays in ESP32 code (Arduino compatible)

---

## Success Criteria

| Criterion | Target | Phase 2.3 |
|-----------|--------|-----------|
| OLED displays Doom scene | Yes | TBD |
| FPS ≥ 15 | ≥15 | TBD |
| Heap free ≥ 300KB | ≥300KB | TBD |
| Compilation | 0 errors | TBD |
| Runtime stability | >10 min | TBD |
| Flash usage | <24% | TBD (est. 23.5%) |
| No input handling | 0 lines | TBD (confirmed) |
| No audio | 0 lines | TBD (confirmed) |

---

## Timeline Estimate

| Phase | Duration | Effort |
|-------|----------|--------|
| Code extraction & analysis | 30 min | Reading code |
| Type system adaptation | 30 min | Minor rewrites |
| Function integration (iter 1-2) | 2-3 hours | Core rendering |
| Testing & validation | 1-2 hours | Hardware validation |
| Documentation | 1 hour | Reports |
| **Total** | **5-6 hours** | **Moderate** |

---

## Decision Point

**Go/No-Go Criteria for Phase 2.3**:

Before starting Phase 2.3, verify:
- [ ] Phase 2.2 hardware test complete (>10 min stable)
- [ ] All Phase 2.2 metrics confirmed
- [ ] No critical issues discovered
- [ ] Team/user explicitly requests Phase 2.3

**Current Status**: Phase 2.2 ✅ COMPLETE  
**Next Action**: Await Phase 2.3 approval

---

## Quick Start (When Ready)

To begin Phase 2.3:

1. Review `third_party/doom-nano/display.h` (277 lines)
2. Extract `renderMap()`, `renderEntities()`, `renderGun()`
3. Adapt to use `g_doomFramebuffer` (from Phase 2.2)
4. Integrate into `DoomAdapter::renderFrame()` (phase 2.2 pattern)
5. Test on hardware (should see Doom maze on OLED)
6. Iterate until stable

---

## Conclusion

**Phase 2.3 is technically feasible**:

✅ Code porting is straightforward (mostly arithmetic)  
✅ Memory budget adequate (+3.5KB estimated)  
✅ Performance headroom sufficient (5ms → raycasting possible)  
✅ Framework proven (Phase 2.2 architecture ready)  
✅ Clear roadmap (extraction → adaptation → integration)

**Status**: Ready to proceed when explicitly requested

**Risk Level**: LOW (incremental, proven patterns)  
**Estimated Effort**: 5-6 hours across 4 iterations

---

*Phase 2.3 planning document complete.*  
*Awaiting approval to proceed.*
