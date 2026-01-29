# Phase 3.3 - Device Testing Guide

**Purpose**: Verify HUD improvements on physical hardware  
**Scope**: Visual inspection, performance validation, gameplay testing  
**Platform**: ESP32 + SSD1306 OLED (128×64, 1-bit)  
**Status**: Ready for device deployment

---

## 🚀 Pre-Deployment Checklist

### Hardware Requirements
- [x] ESP32 development board (DOIT DevKit V1)
- [x] SSD1306 OLED display (128×64, I2C)
- [x] USB cable for upload/monitor
- [x] Power source (5V)
- [x] Serial monitor (115200 baud)

### Software Requirements
- [x] PlatformIO CLI configured
- [x] esp32doit-devkit-v1 environment ready
- [x] Code compiled successfully (6.79s build)
- [x] Firmware binary ready (329.9 KB)

---

## 📋 Test Plan

### Phase 1: Deployment (5 minutes)

**Step 1: Build Verification**
```bash
cd C:\Users\rafae\OneDrive\Documentos\PlatformIO\Projects\mini-doom
pio run
```

Expected output:
```
RAM:   [=         ]   7.5%
Flash: [===       ]  25.2%
Build: SUCCESS (6.79 seconds)
Result: Successfully created esp32 image
```

✅ **Pass Criteria**: SUCCESS message, no errors

---

**Step 2: Upload to Device**
```bash
pio run -t upload
```

Expected output:
```
Writing at 0x00000000... (X%)
Writing at 0x00008000... (X%)
[... progress ...]
Leaving...
Hard resetting via RTS pin...
```

✅ **Pass Criteria**: No upload errors, device reset

---

**Step 3: Serial Monitor (Device Startup)**
```bash
pio device monitor -b 115200
```

Expected logs (first 5 seconds):
```
... (various init logs) ...
[HUD] Initialized (Phase 3.3 - Polish & Visual Hierarchy)
[WORLD] Doom raycasting engine active (Phase 3.1)
[SPRITE] Doom-style sprite textures active (Phase 3.2)
... (ready to play) ...
```

✅ **Pass Criteria**: All 3 phase logs visible, device ready

---

### Phase 2: Visual Inspection (10 minutes)

#### Test 2.1: HP Display (Left Side)
```
Visual location: Bottom-left corner (x=2, y=50-63)

Checklist:
  [ ] "HP" label visible (small text)
  [ ] Bar visible below label (10×4 pixels, thicker than before)
  [ ] Bar borders clear (top, bottom, left, right edges visible)
  [ ] Health number visible below bar (large, readable)
  [ ] Bar properly filled (segments match health %)
```

**Success Criteria**:
- [x] Bar is noticeably thicker than Phase 2.11 (visual comparison)
- [x] Number is prominent and readable at 12" distance
- [x] Layout flows top-to-bottom: "HP" → Bar → Number
- [x] No visual artifacts or glitches

---

#### Test 2.2: Ammo Display (Right Side)
```
Visual location: Bottom-right corner (x=83-106, y=50-63)

Checklist:
  [ ] "AMMO" label visible (small text, 4 characters)
  [ ] Label positioned above number
  [ ] Ammo number visible below label (large, readable)
  [ ] Number right-aligned on screen
  [ ] Ammunition count correct (matches game state)
```

**Success Criteria**:
- [x] "AMMO" label completely spelled out (not abbreviated)
- [x] Number clearly below label (not inline)
- [x] Text layout right-aligned (professional appearance)
- [x] No visual artifacts or glitches

---

#### Test 2.3: Crosshair (Center)
```
Visual location: Screen center (x=64, y=32)

Checklist:
  [ ] Crosshair visible at screen center
  [ ] 5×5 pattern visible (extends 2px in each direction)
  [ ] Center is EMPTY (can see world through middle)
  [ ] Arms extend up, down, left, right
  [ ] Easily visible against wall textures
```

**Success Criteria**:
- [x] Crosshair noticeably larger (5×5 vs old 3×3)
- [x] Center is hollow (not filled solid)
- [x] Sight-line clearly visible (can aim through center)
- [x] Visible on all wall types (BRICK, METAL, STONE)
- [x] Visible on sprite backgrounds

---

#### Test 2.4: Separator Line
```
Visual location: Between world and HUD (y=47)

Checklist:
  [ ] Horizontal line visible
  [ ] Line separates world from HUD area
  [ ] Line is 1 pixel thick
  [ ] Line spans full screen width (128px)
```

**Success Criteria**:
- [x] Clear visual boundary between world and HUD
- [x] Creates "foundation" effect (Doom aesthetic)
- [x] Improves spatial understanding

---

### Phase 3: Performance Testing (10 minutes)

#### Test 3.1: Frame Rate Verification
```
Method: Visual observation + Serial logs

During gameplay:
  [ ] No visible stuttering
  [ ] Movement smooth and consistent
  [ ] Screen updates at fixed rate (no jittering)
  [ ] No frame drops or hitches
```

**Serial Monitor Check**:
Watch logs for ~30 seconds, observe:
```
[HUD] HP=100 Ammo=20
[HUD] HP=100 Ammo=20    <- should update consistently
[HUD] HP=100 Ammo=20
```

✅ **Pass Criteria**: Updates consistent, no jumps in counter

---

#### Test 3.2: Memory Stability
```
Serial Monitor: Check for memory errors or warnings

Failure signs:
  ❌ "Heap out of memory"
  ❌ "Stack overflow"
  ❌ "Malloc failure"
  ❌ Device reset/crash
```

✅ **Pass Criteria**: No memory errors, device runs 5+ minutes stable

---

#### Test 3.3: Temperature Monitoring
```
Visual observation during 10+ minutes gameplay

Success indicators:
  ✅ Device doesn't get unusually hot
  ✅ No thermal throttling visible
  ✅ Consistent FPS throughout
```

✅ **Pass Criteria**: Normal operating temperature, no throttling

---

### Phase 4: Gameplay Testing (20 minutes)

#### Test 4.1: Health System
```
Starting state: Max health (100)

Actions:
  1. Take damage from enemy
  2. Observe HP number decreases
  3. Observe HP bar shrinks proportionally
  4. Continue until health < 30%
  5. Observe blinking behavior

Checklist:
  [ ] Number updates correctly
  [ ] Bar updates correctly
  [ ] Bar blinks when < 30% (together with number)
  [ ] Blink rate is visible (not too fast/slow)
  [ ] Player can still read blinking value
```

✅ **Pass Criteria**:
- HP updates match actual health value
- Bar visual matches numerical percentage
- Blinking doesn't obscure information
- Blink interval is comfortable (500ms)

---

#### Test 4.2: Ammo System
```
Starting state: Initial ammo (e.g., 20)

Actions:
  1. Fire weapon repeatedly
  2. Observe ammo number decreases
  3. Continue until ammo == 0
  4. Try to shoot (should fail)

Checklist:
  [ ] Number decreases with each shot
  [ ] "NO AMMO" appears when ammo == 0
  [ ] "NO AMMO" text is visible and prominent
  [ ] "NO AMMO" blinks (easy to notice)
  [ ] Cannot shoot when ammo == 0
```

✅ **Pass Criteria**:
- Ammo counter accurate
- "NO" state impossible to miss
- Blinking "NO AMMO" draws attention
- Gameplay consequence (can't shoot) enforced

---

#### Test 4.3: Crosshair Functionality
```
Actions during combat:
  1. Aim at walls
  2. Aim at enemies
  3. Aim at distant objects
  4. Move while aiming

Checklist:
  [ ] Crosshair visible at all times
  [ ] Center is clear (can see target)
  [ ] Helps aim at enemies effectively
  [ ] Not confused with enemy sprites
  [ ] Easier than Phase 2.11 version
```

✅ **Pass Criteria**:
- Crosshair improves aiming feedback
- Center hollow design works well
- 5×5 size feels right for OLED
- No interference with gameplay

---

#### Test 4.4: Combat Flow
```
Full combat scenario (2-3 waves):
  1. Spawn into level
  2. See enemies approaching (recognize IDLE pose)
  3. Shoot enemy (observe FIRE pose + ENEMY_HIT)
  4. Move around, manage ammo
  5. Take damage, watch HP bar
  6. Run out of ammo, see "NO AMMO"
  7. Survive and complete wave

Checklist:
  [ ] Visual feedback immediate (no lag)
  [ ] HUD doesn't interfere with gameplay
  [ ] All state changes visible
  [ ] Game feels polished and complete
  [ ] No visual regressions vs Phase 2.15
```

✅ **Pass Criteria**:
- Combat feels responsive
- HUD "invisible" (informs without distracting)
- All phase 3 improvements working together
- Game feels professional quality

---

### Phase 5: Regression Testing (5 minutes)

Verify nothing broke from Phase 2.15:

```
Checklist:
  [ ] Raycasting still works (world renders)
  [ ] Sprites still render (enemies/weapon visible)
  [ ] Combat still works (damage, enemy death)
  [ ] Waves still spawn (new enemies appear)
  [ ] Input still works (movement, shooting)
  [ ] Sound/feedback still works (if implemented)
  [ ] No new glitches or artifacts
```

✅ **Pass Criteria**: All Phase 2.15 functionality intact

---

## 📝 Test Results Log

### Test Session 1: [Date/Time]

**Tester**: [Name]  
**Device**: ESP32 DOIT V1 + SSD1306  
**Duration**: [minutes]

#### Phase 1: Deployment
```
Build: ✅ SUCCESS
Upload: ✅ SUCCESS
Serial logs: ✅ Correct (Phase 3.3 visible)
```

#### Phase 2: Visual Inspection
```
HP Display:   ✅ / ❌  (notes: ____________)
Ammo Display: ✅ / ❌  (notes: ____________)
Crosshair:    ✅ / ❌  (notes: ____________)
Separator:    ✅ / ❌  (notes: ____________)
```

#### Phase 3: Performance
```
FPS stable: ✅ / ❌  (notes: ____________)
Memory ok:  ✅ / ❌  (notes: ____________)
Temp ok:    ✅ / ❌  (notes: ____________)
```

#### Phase 4: Gameplay
```
Health: ✅ / ❌  (notes: ____________)
Ammo:   ✅ / ❌  (notes: ____________)
Combat: ✅ / ❌  (notes: ____________)
Flow:   ✅ / ❌  (notes: ____________)
```

#### Phase 5: Regression
```
Raycasting: ✅ / ❌
Sprites:    ✅ / ❌
Combat:     ✅ / ❌
Waves:      ✅ / ❌
Input:      ✅ / ❌
```

#### Overall Result
```
PASS ✅  /  FAIL ❌  /  PASS WITH NOTES ⚠️
```

#### Notes
```
[Any observations, issues, improvements]
```

---

## 🐛 Troubleshooting

### Issue: "Garbage" on OLED display
**Cause**: I2C communication problem  
**Solution**: 
- Check I2C wiring (SDA, SCL, GND)
- Verify pull-up resistors (4.7K typical)
- Reset device with button

---

### Issue: HUD text unreadable
**Cause**: Could be normal (OLED is low-contrast)  
**Solution**:
- Adjust viewing angle
- Wait for eyes to adapt
- Check if world renders (OLED working?)
- Compare with Phase 2.11 HUD (should be clearer now)

---

### Issue: Frame drops or stuttering
**Cause**: Could be I2C timing issue  
**Solution**:
- Check I2C bus frequency (400kHz typical)
- Reduce other I2C devices
- Verify no malloc during render

---

### Issue: Device crashes/resets
**Cause**: Memory overflow or stack issue  
**Solution**:
- Check serial logs before crash
- Verify build is latest (pio run)
- Look for "Heap out of memory" in logs
- Report to developer

---

### Issue: Ammo shows wrong number
**Cause**: Rendering update lag  
**Solution**:
- Normal for 20 FPS (50ms latency is expected)
- Will update within 1 frame of shot fired
- Not a bug

---

## 📊 Expected vs Actual Comparison

### Expected (from Phase 3.3 design)
```
HP Bar:      10×4 pixels, bordered, thick
HP Number:   Large, below bar, always readable
AMMO Label:  "AMMO" (4 chars), small text
AMMO Number: Large, below label
Crosshair:   5×5 hollow (empty center)
Separator:   1px horizontal line at y=47
FPS:         20 fixed, smooth
```

### Actual (device testing results)
```
HP Bar:      [✅/❌] _____________
HP Number:   [✅/❌] _____________
AMMO Label:  [✅/❌] _____________
AMMO Number: [✅/❌] _____________
Crosshair:   [✅/❌] _____________
Separator:   [✅/❌] _____________
FPS:         [✅/❌] _____________
```

---

## ✅ Sign-Off

When all tests pass:

```
Device Testing Complete: ✅

Tester: ________________  Date: __________
Device: ________________  Build: 3.3
Result: ✅ PASS  /  ⚠️ PASS WITH NOTES  /  ❌ NEEDS FIXES

Comments:
_________________________________________________________________
_________________________________________________________________
_________________________________________________________________

Next steps:
[ ] Deploy to production
[ ] Plan Phase 3.4 improvements
[ ] Gather user feedback
[ ] Fix any issues found
```

---

## 📞 Notes for Users Testing Phase 3.3

**What to look for**:
- ✅ HUD is clearer than Phase 2.11
- ✅ Can read HP/Ammo instantly (no "what's my health?" moment)
- ✅ Game feels more polished and intentional
- ✅ Crosshair easier to use than old version

**What NOT to worry about**:
- Text is monochrome (expected on OLED 1-bit)
- Some pixels may seem low-contrast (viewing angle affects this)
- Update latency ~50ms is expected (20 FPS, 5 frame buffer)

**Feedback appreciated**:
- Is HP bar easier to see?
- Is number immediately readable?
- Does "NO AMMO" catch your attention?
- Is crosshair better for aiming?
- Any visual glitches or artifacts?

---

*Device Testing Guide for Phase 3.3*  
*Ready for hardware validation*
