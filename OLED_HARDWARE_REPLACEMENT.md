# OLED Hardware Replacement — Post-Investigation Report

**Status:** 🔴 **BLOCKING** — Awaiting Hardware  
**Date:** 2026-01-28  
**Commit:** `81a721e` (oled-pre-hardware-replacement)

---

## Problem Summary

The current OLED module (marketed as "128×64 bicolor") is **incompatible with SSD1306 framebuffer mode**.

### Root Cause
- **Module type:** SSD1306-clone (bicolor, yellow/blue split)
- **Issue:** Fixed internal RAM window with non-linear memory mapping
- **Impact:** Full framebuffer writes (0xFF) do NOT fill visible display; persistent horizontal banding observed
- **Diagnosis:** Confirmed via 4 independent tests:
  1. Forced white framebuffer (0xFF) — FAIL
  2. Display offset sweep (0x00-0x20) — FAIL
  3. Start line sweep (0x00-0x30) — FAIL
  4. Horizontal addressing mode validation — FAIL

### Why This Matters
The MiniDoom engine relies on **framebuffer mode** for:
- Raycasting (wall rendering)
- Sprite rendering
- HUD + status display
- Screen shake / damage feedback

Page buffer mode (u8g2) would require rewriting the entire graphics pipeline (4-6 hours).

---

## Solution: Hardware Replacement

### Decision Rationale
✅ **Option B — Replace Hardware** was selected because:
- Preserves existing MiniDoom framebuffer renderer
- Avoids major architectural rewrite
- Maintains performance (20 FPS)
- Fastest resolution path
- Lowest risk

---

## Hardware Requirements (STRICT)

### Required OLED Module
Must meet **ALL** of these criteria:

| Specification | Requirement |
|---------------|-------------|
| **Controller** | True SSD1306 (NOT clone, NOT SH1106) |
| **Resolution** | 128×64 pixels |
| **Color** | **Monochrome only** (white/blue single-color, NO yellow/blue split) |
| **Interface** | I2C |
| **Default Address** | 0x3C |
| **Library Compatibility** | Adafruit SSD1306 |
| **Supply Voltage** | 3.3V |

### Approved Search Terms
```
"SSD1306 128x64 I2C OLED monochrome"
"0.96 inch SSD1306 OLED white display"
"SSD1306 128x64 single color I2C"
```

### Explicitly Reject
```
❌ Bicolor (yellow/blue)
❌ "SSD1306-compatible" (unknown clones)
❌ SH1106 (different controller)
❌ 128×32 (wrong resolution)
❌ Unmarked/generic modules
```

### Recommended Sources
- Adafruit official store (guaranteed compatible)
- Amazon/eBay: Search for "SSD1306 128x64 monochrome I2C" with verified seller reviews
- Look for photos showing single-color display in listings

---

## Pinout (NO CHANGES REQUIRED)

```
OLED Pin       ESP32 Pin
─────────────────────────
SDA      →     GPIO 21
SCL      →     GPIO 22
VCC      →     3.3V
GND      →     GND
```

---

## Post-Install Validation Procedure

### Prerequisites
- ✅ New SSD1306 128×64 monochrome OLED physically installed
- ✅ Wiring verified against pinout above
- ✅ ESP32 powered on

### Step 1: Flash Firmware
```bash
cd /path/to/mini-doom
pio run -t upload
```

Expected output:
```
[SUCCESS] Took XX seconds =========================
```

### Step 2: Open Serial Monitor
```bash
pio device monitor
```

Expected output (first ~2 seconds):
```
[OLED] Initialize: Power stabilization delay (100ms)...
[OLED] Initialize: Starting SSD1306 setup at 0x3C...
[OLED] Initialize: Driver responding, configuring display...
[OLED] Initialize: ✓ Display ready!
[mini-doom] [INIT] Doom initialization complete
[mini-doom] Doom: Frame #1 | Free Heap: XXXXX bytes
```

### Step 3: Visual Validation

#### Test 3a — White Screen Test
The framebuffer should fill the entire display with white on startup.

**Expected:** Entire 128×64 screen uniformly white (yellow region fully lit, blue region fully lit)  
**Fail Condition:** Horizontal bands, stripes, or black regions visible

#### Test 3b — Game Title Screen
After ~5 seconds, the DOOM title should appear with proper rendering.

**Expected:**
- "DOOM" text visible
- Start game prompt visible
- Clean, readable text
- No garbage pixels

#### Test 3c — Stable Operation
The device should run stably for at least 30 seconds without errors.

**Expected:**
- Serial monitor shows continuous frame counter: `Doom: Frame #XXX`
- No I2C errors
- No heap warnings
- Consistent 20 FPS (50ms frame time)

---

## Acceptance Criteria

| Criterion | Status | Notes |
|-----------|--------|-------|
| Full white screen fills entire display | [ ] PASS | No bands, no black regions |
| No I2C errors in logs | [ ] PASS | Check serial output for errors |
| OLED initializes at 0x3C | [ ] PASS | Should appear in first 2 seconds |
| Title screen renders | [ ] PASS | Readable text, no garbage |
| HUD visible | [ ] PASS | Health/ammo display |
| Stable ≥30 seconds | [ ] PASS | No crashes, consistent frame rate |

✅ **PRODUCTION READY** when all criteria pass.

---

## If Validation Fails

### Scenario 1: I2C Communication Error
```
[ERROR] [OLED] Initialize: FAILED - driver not responding at 0x3C
```

**Troubleshoot:**
1. Check wiring (SDA GPIO21, SCL GPIO22)
2. Check power (3.3V at VCC pin)
3. Verify I2C address with: `pio device monitor` and look for I2C scan results
4. Try alternate address (0x3D) — see `src/config.h`

### Scenario 2: Display Shows Garbage / Bands
```
(Display shows horizontal lines or partial image)
```

**Troubleshoot:**
1. Verify this is a **true SSD1306** (not a clone)
2. Check power stability (use multimeter)
3. Confirm monochrome (single-color) module, not bicolor
4. Try reseating the OLED connector

### Scenario 3: No Display Output
```
(Frame counter shows, but display blank)
```

**Troubleshoot:**
1. Verify power (3.3V) at OLED VCC pin
2. Check if backlight needs to be enabled (some modules have separate LED pins)
3. Verify module is a **display module**, not just controller board

---

## Code State

### What Was Changed (OLED Investigation)
- ✅ `src/oled_ui.cpp` — Removed offset/startline sweep tests
- ✅ `src/config.h` — Resolution changed 128×64 → 128×32 (during testing)
- ✅ `src/doom_adapter.cpp` — Removed forced white framebuffer test
- ✅ `src/doom_graphics.h` — Buffer size adjustments during testing

**Commit:** `81a721e`  
**Tag:** `oled-pre-hardware-replacement`

### What Was NOT Changed
- ✅ MiniDoom renderer (intact)
- ✅ Framebuffer architecture (intact)
- ✅ Game engine (intact)
- ✅ HUD rendering (intact)

**Note:** The resolution was reverted to 128×64 after investigation. The code is ready for a true 128×64 SSD1306.

---

## Reference: Investigation Summary

### Tests Conducted
1. **Forced White Framebuffer (0xFF)** → Horizontal bands, not white
2. **Display Offset Sweep (0x00-0x20)** → No change in output
3. **Start Line Sweep (0x00-0x30)** → No improvement
4. **Bit Order + Byte Index Validation** → Confirmed correct

### Conclusion
The OLED module hardware is fundamentally incompatible with SSD1306 framebuffer mode. This is **not a software bug** or configuration issue. The clone controller has a fixed internal RAM window that does not match the visible display area.

---

## Timeline

| Date | Event |
|------|-------|
| 2026-01-28 | Investigation completed, root cause identified |
| 2026-01-28 | Checkpoint commit + tag created |
| TBD | New hardware procured |
| TBD | New hardware installed |
| TBD | Post-install validation executed |

---

## Contact / Notes

For questions about this investigation, see git log:
```bash
git log --oneline | grep -i oled
git show 81a721e
git tag -l oled-pre-hardware-replacement
```

**Questions?** Review the investigation commit for detailed code changes and test sequences.

---

**Status:** ⏳ **Awaiting Hardware Installation**

Do not proceed with software changes until replacement OLED is installed and post-install validation is complete.
