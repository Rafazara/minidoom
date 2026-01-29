# VISUAL TEST PATTERNS - WHAT YOU SHOULD SEE ON OLED

## Test 0: All Black

**Command:** `0`  
**Expected:** Completely dark/black screen  
**Visual:**
```
████████████████████████████ (128 pixels wide)
████████████████████████████
████████████████████████████
████████████████████████████
████████████████████████████
████████████████████████████
████████████████████████████
████████████████████████████ (64 pixels tall)
```

**What to verify:**
- [ ] OLED is completely black
- [ ] No visible pixels
- [ ] No artifacts or noise

**If this fails:** OLED not responding to framebuffer writes → Check I2C wiring

---

## Test 1: All White

**Command:** `1`  
**Expected:** Completely bright/white screen  
**Visual:**
```
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ (128 pixels wide)
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ (64 pixels tall)
```

**What to verify:**
- [ ] OLED is completely bright/white
- [ ] Uniform brightness across entire screen
- [ ] No dark spots or artifacts

**If this fails:** Framebuffer not being flushed → Check blitBufferToOLED()

---

## Test 2: Pixel at (0,0) - Top-Left Corner

**Command:** `2`  
**Expected:** Single bright pixel at the **very top-left corner**  
**Visual:**
```
●                           (single bright dot at top-left)
                            (rest of screen black)
```

**Exact placement:** Pixel should be at the extreme top-left, no offset.

**What to verify:**
- [ ] Single pixel visible
- [ ] Pixel at TOP-LEFT corner (not top-center, not slightly offset)
- [ ] Rest of screen black

**If pixel is in wrong location:** Bit order is reversed → Apply Fix 1.1

---

## Test 3: Pixel at (127,0) - Top-Right Corner

**Command:** `3`  
**Expected:** Single bright pixel at the **very top-right corner**  
**Visual:**
```
                           ● (single bright dot at top-right)
                            (rest of screen black)
```

**Exact placement:** Pixel should be at the extreme top-right, no offset.

**What to verify:**
- [ ] Single pixel visible
- [ ] Pixel at TOP-RIGHT corner (not top-center, not slightly offset)
- [ ] Rest of screen black

**If pixel is in wrong location:** Bit order is reversed → Apply Fix 1.1

---

## Test 4: Pixel at (0,63) - Bottom-Left Corner

**Command:** `4`  
**Expected:** Single bright pixel at the **very bottom-left corner**  
**Visual:**
```
                            (rest of screen black)
                            (rest of screen black)
                            
●                           (single bright dot at bottom-left)
```

**Exact placement:** Pixel should be at the extreme bottom-left, no offset.

**What to verify:**
- [ ] Single pixel visible
- [ ] Pixel at BOTTOM-LEFT corner (not bottom-center, not slightly offset)
- [ ] Rest of screen black

**If pixel is in wrong location:** Bit order is reversed → Apply Fix 1.1

---

## Test 5: Pixel at (127,63) - Bottom-Right Corner

**Command:** `5`  
**Expected:** Single bright pixel at the **very bottom-right corner**  
**Visual:**
```
                            (rest of screen black)
                            (rest of screen black)
                            
                           ● (single bright dot at bottom-right)
```

**Exact placement:** Pixel should be at the extreme bottom-right, no offset.

**What to verify:**
- [ ] Single pixel visible
- [ ] Pixel at BOTTOM-RIGHT corner (not bottom-center, not slightly offset)
- [ ] Rest of screen black

**If pixel is in wrong location:** Bit order is reversed → Apply Fix 1.1

---

## Test 6: Pixel at (64,32) - Center

**Command:** `6`  
**Expected:** Single bright pixel at the **exact center** of screen  
**Visual:**
```
                            (rest of screen black)
                            
                    +       (single bright dot at center)
                            
                            (rest of screen black)
```

**Exact placement:** Pixel should be exactly at (64, 32), the center point.

**What to verify:**
- [ ] Single pixel visible
- [ ] Pixel at exact CENTER (horizontally and vertically)
- [ ] Rest of screen black

**If pixel is offset from center:** Coordinate calculation wrong → Check framebuffer byte indexing

---

## Test 7: Horizontal Lines (8 Stripes)

**Command:** `7`  
**Expected:** 8 evenly-spaced horizontal stripes across the screen  
**Visual:**
```
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 1
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 2
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 3
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 4
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 5
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 6
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 7
        (8 pixels black)
■■■■■■■■■■■■■■■■■■■■■■■■■■ ← Line 8
```

**What to verify:**
- [ ] 8 stripes visible
- [ ] All stripes same thickness (8 pixels)
- [ ] All stripes equally spaced (8-pixel gaps)
- [ ] Lines solid (no gaps or break)
- [ ] Lines span full width (128 pixels)

**If stripes are broken or offset:** Byte indexing calculation wrong

---

## Test 8: Vertical Lines (16 Stripes)

**Command:** `8`  
**Expected:** 16 evenly-spaced vertical stripes across the screen  
**Visual:**
```
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │ (16 vertical stripes)
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
│ │ │ │ │ │ │ │ │ │ │ │ │ │ │ │
```

**What to verify:**
- [ ] 16 stripes visible
- [ ] All stripes same width (8 pixels)
- [ ] All stripes equally spaced (8-pixel gaps)
- [ ] Lines solid (no gaps or break)
- [ ] Lines span full height (64 pixels)

**If stripes are broken or offset:** Byte indexing calculation wrong

---

## Test 9: Grid Pattern (8×8)

**Command:** `9`  
**Expected:** Regular grid of 8×8 cells (combination of tests 7 + 8)  
**Visual:**
```
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
```

**What to verify:**
- [ ] Regular grid visible
- [ ] All grid cells same size (8×8 pixels)
- [ ] Horizontal lines equally spaced
- [ ] Vertical lines equally spaced
- [ ] Lines solid and straight

**If grid is skewed or cells unequal:** Byte indexing or coordinate calculation wrong

---

## Test 10 (A): Checkerboard Pattern

**Command:** `a`  
**Expected:** Fine checkerboard pattern (alternating pixels)  
**Visual:**
```
●○●○●○●○●○●○●○●○●○●○●○●○●○●○●○ ← Pattern alternates every pixel
○●○●○●○●○●○●○●○●○●○●○●○●○●○●○●
●○●○●○●○●○●○●○●○●○●○●○●○●○●○●○
○●○●○●○●○●○●○●○●○●○●○●○●○●○●○●
●○●○●○●○●○●○●○●○●○●○●○●○●○●○●○
○●○●○●○●○●○●○●○●○●○●○●○●○●○●○●
●○●○●○●○●○●○●○●○●○●○●○●○●○●○●○
○●○●○●○●○●○●○●○●○●○●○●○●○●○●○●
```

**What to verify:**
- [ ] Checkerboard pattern is FINE (individual pixel alternation)
- [ ] Pattern is CRISP (no blurry or merged pixels)
- [ ] Pattern is REGULAR (consistent throughout)
- [ ] No blank areas or corruption

**If pattern looks blocky or wrong:** Pixel addressing still wrong

---

## Test 11 (B): Gradient (5-Level Intensity)

**Command:** `b`  
**Expected:** Smooth fade from light (top) to dark (bottom) with ~5 visible intensity levels  
**Visual:**
```
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ ← Level 5 (brightest - most pixels on)

░░░░░░░░░░░░░░░░░░░░░░░░░░░░ ← Level 4 (medium-bright)

░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ░ ← Level 3 (medium)

  ▓   ▓   ▓   ▓   ▓   ▓        ← Level 2 (medium-dark)

                              ← Level 1 (darkest - few pixels)
```

**What to verify:**
- [ ] Gradient visible from top (bright) to bottom (dark)
- [ ] At least 3-4 distinct intensity levels visible
- [ ] Smooth transition (no sharp jumps between levels)
- [ ] No horizontal banding artifacts

**Note:** This test is cosmetic - if checkerboard passed, this should show at least some gradient effect

---

## Test 12 (C): Complete Scene

**Command:** `c`  
**Expected:** Full game scene with walls, HUD, crosshair, and all UI elements  
**Visual:**
```
┌──────────────────────────────────────┐
│ ╔═════════════════════════════════╗  │
│ ║                                 ║  │
│ ║             +                   ║  │ ← Game world area
│ ║         (crosshair)             ║  │    (walls on left/right)
│ ║                                 ║  │
│ ║                                 ║  │
│ ╠═════════════════════════════════╣  │
│ ║ [H] 100/100            [A] 50  ║  │ ← HUD bar
│ ╚═════════════════════════════════╝  │
└──────────────────────────────────────┘
```

**What to verify:**
- [ ] Walls visible on left and right edges
- [ ] Crosshair (+) centered on screen
- [ ] HUD bar at bottom with text/indicators
- [ ] All elements properly positioned
- [ ] No overlap or corruption between layers

**If any element missing:** Check individual rendering functions

---

## Test 13 (D): Scrolling Animation

**Command:** `d`  
**Expected:** Animated scrolling pattern (repeats every ~1 second)  
**Visual:**
```
Frame 1: ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆
         ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆

Frame 2:  ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆
          ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆

Frame 3: ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆
         ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆ ┆
```

**What to verify:**
- [ ] Animation is SMOOTH (no flicker or jitter)
- [ ] Pattern scrolls consistently
- [ ] NO TEARING (pattern stays aligned, doesn't break mid-update)
- [ ] Animation repeats correctly

**If animation has artifacts:** Frame synchronization issue or buffer corruption

---

## Master Verification Checklist

### Connectivity Check (Tests 0-1)
- [ ] Test 0 (All Black): Screen completely dark
- [ ] Test 1 (All White): Screen completely bright

### Coordinate Mapping (Tests 2-6)
- [ ] Test 2 (Top-Left): Pixel at TOP-LEFT corner
- [ ] Test 3 (Top-Right): Pixel at TOP-RIGHT corner
- [ ] Test 4 (Bottom-Left): Pixel at BOTTOM-LEFT corner
- [ ] Test 5 (Bottom-Right): Pixel at BOTTOM-RIGHT corner
- [ ] Test 6 (Center): Pixel at CENTER of screen

### Rendering (Tests 7-9)
- [ ] Test 7 (H-Lines): 8 horizontal stripes, evenly spaced
- [ ] Test 8 (V-Lines): 16 vertical stripes, evenly spaced
- [ ] Test 9 (Grid): Regular 8×8 grid pattern

### Intensity & Dithering (Tests 10-11)
- [ ] Test 10 (Checkerboard): Fine checkerboard, not blocky
- [ ] Test 11 (Gradient): Smooth fade from light to dark

### Complete Pipeline (Tests 12-13)
- [ ] Test 12 (Scene): All elements visible and properly positioned
- [ ] Test 13 (Animation): Smooth scrolling without tearing

---

## If Any Test Fails

1. **Tests 0-1 fail:** I2C not connected
   - Check: SDA (GPIO21), SCL (GPIO22), GND, 3.3V on OLED

2. **Tests 2-6 fail (wrong positions):** Bit order wrong
   - Fix: Apply Fix 1.1 and 1.2 in doom_graphics.cpp and doom_world_renderer.cpp

3. **Tests 7-9 fail (broken lines):** Framebuffer indexing wrong
   - Check: Byte formula `(y/8)*128+x` in doom_graphics.cpp

4. **Tests 10-11 fail:** Pixel addressing still incorrect
   - Fix: Re-check bit order fix (Fix 1.1)

5. **Test 12 fails (elements missing):** Layer rendering issue
   - Check: Wall renderer, sprite renderer, HUD functions

6. **Test 13 fails (tearing/flicker):** Display sync issue
   - Check: blitBufferToOLED() called every loop iteration

---

## Quick Validation Flow

```
Start
 ├─ Tests 0-1 pass? → NO → I2C Problem (STOP)
 │                  → YES ↓
 ├─ Tests 2-6 all in correct positions? → NO → Bit Order Wrong (Apply Fix)
 │                                     → YES ↓
 ├─ Tests 7-9 lines aligned? → NO → Indexing Wrong (Check Formula)
 │                            → YES ↓
 ├─ Tests 10-11 patterns visible? → NO → Address Still Wrong (Re-check Fix)
 │                                 → YES ↓
 ├─ Test 12 complete scene visible? → NO → Layer Problem (Check Renderers)
 │                                  → YES ↓
 ├─ Test 13 smooth animation? → NO → Sync Issue (Check Blit Call)
 │                             → YES ↓
 └─ ✅ PIPELINE VALID - Enable Gameplay
```

---

## Notes

- Tests 0-6 are **critical** (connectivity and coordinate validation)
- Tests 7-9 are **important** (rendering correctness)
- Tests 10-11 are **optional** (intensity is visual polish, not required)
- Test 12 is **final integration** (all layers working)
- Test 13 is **frame coherence** (display refresh validation)

Once all tests pass, display pipeline is 100% correct and ready for gameplay.

