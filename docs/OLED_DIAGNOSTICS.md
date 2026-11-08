# OLED diagnostics

## Investigation status

The OLED responds at `0x3C` from a previous physical scan, and earlier tests produced illuminated but fragmented/horizontal output. The user has now confirmed that the corrected OLED diagnostic firmware works on the physical device. This establishes general physical validation of the diagnostic path; it does not archive a pattern-by-pattern checklist or validate gameplay, controls, timing, or stability.

An older hardware report concluded that a bicolor SSD1306 clone was responsible. That report is historical evidence, not proof of the identity or behavior of the currently attached module. The diagnostic below is intended to separate software mapping from panel/controller problems.

## Confirmed software findings

### Mixed framebuffer representations

The game and overlays use 128×64 SSD1306 page data. An earlier blit passed that buffer to `drawBitmap()`, which expects row-major 1-bit data. The recovery branch already replaced that call with an exact copy to the Adafruit buffer. All inspected game renderers now use the same page offset and LSB-first vertical bit order.

### 512-byte destination overflow

Before this recovery, `config.h` constructed the Adafruit display as 128×32. Its internal buffer was therefore 512 bytes, while the new game blit copied 1,024 bytes. This was a confirmed 512-byte heap overwrite and also selected the wrong SSD1306 COM-pin initialization. The display height is now 64, and compile-time assertions require matching dimensions.

This confirmed defect was a plausible cause of corruption. The later user-confirmed diagnostic success supports the correction, but does not validate gameplay or replace the detailed visual checklist.

### Inconsistent orientation handling

The old initialization selected GFX rotation 2 and then overrode the controller remap/COM commands. Raw framebuffer copies ignore GFX coordinate rotation, while GFX drawing applies it. This made text and raw game coordinates follow different conventions. Initialization now uses rotation 0 consistently; the resulting default SSD1306 remap/COM direction matches the previous final raw-framebuffer command state.

### Missing display flushes

Title and game-over states generated pixels but returned before the only blit call. Both states now flush their framebuffers. `DoomGraphics::initialize()` is also called during boot.

### Map decoding mismatch

World rendering and collision disagreed about row order and nibble order. Both now use the shared Doom Nano decoder, which has dedicated host-test coverage. This affects world/collision correctness, not the electrical integrity of the OLED.

## Isolated diagnostic mode

The diagnostic environment initializes only Wire, OLEDUI, DoomGraphics, and the validation-pattern path. It does not initialize game state, enemies, input, AI, or combat.

Build only:

```powershell
pio run -e esp32doit-devkit-v1-oled-diag
```

After explicit approval to flash:

```powershell
pio run -e esp32doit-devkit-v1-oled-diag -t upload
pio device monitor -p COM7 -b 115200
```

Return to the game:

```powershell
pio run -e esp32doit-devkit-v1 -t upload
```

## Pattern sequence

Most patterns remain visible for three seconds. The moving square updates every 100 ms for 30 frames before the cycle repeats.

| Order | Pattern | Required observation |
|---:|---|---|
| 1 | All black | Entire panel dark |
| 2 | All white | Entire 128×64 area uniformly lit |
| 3–6 | Individual corner pixels | Exactly one pixel at the named corner |
| 7 | Center pixel | Exactly one pixel at (64,32) |
| 8 | Horizontal lines | Eight evenly spaced full-width lines |
| 9 | Vertical lines | Sixteen evenly spaced full-height lines |
| 10 | Grid | Regular orthogonal grid |
| 11 | Checkerboard | Fine alternating 2×2 blocks |
| 12 | Gradient | Stable dither pattern without random rows |
| 13 | Complete scene | Known synthetic walls/HUD/crosshair |
| 14 | Scrolling pattern | Stable vertical-stripe frame |
| 15 | Border | Continuous one-pixel rectangle on all edges |
| 16 | Corner markers | Distinct square/horizontal/vertical/diagonal markers |
| 17 | Text orientation | Upright `TOP` at upper left |
| 18 | Moving square | 8×8 square moves left to right cleanly |

Serial messages saying a test was generated indicate only framebuffer generation. They are not hardware PASS results.

## Physical result template

Record actual observations, not expected values:

```text
Module photo / markings:
Firmware environment:
Git commit:
I2C address and clock:
All black: PASS / FAIL / NOT RUN — notes
All white: PASS / FAIL / NOT RUN — notes
Corners: PASS / FAIL / NOT RUN — notes
Lines/grid/checkerboard: PASS / FAIL / NOT RUN — notes
Border: PASS / FAIL / NOT RUN — notes
Text orientation: PASS / FAIL / NOT RUN — notes
Moving square: PASS / FAIL / NOT RUN — notes
Normal title: PASS / FAIL / NOT RUN — notes
30-second stability: PASS / FAIL / NOT RUN — notes
Serial errors:
Photos captured:
```

## Interpretation

- **All white fails or bands persist:** suspect panel/controller compatibility, power, or transfer behavior. Do not tune gameplay.
- **Filled screen passes but border/corners are displaced:** investigate address window, column offset, or panel geometry.
- **Geometry passes but text is rotated/mirrored:** investigate orientation using the recorded corner results.
- **Diagnostic passes but game corrupts:** investigate game rendering and runtime memory/timing with the now-matched buffers.
- **Display is stable but FPS is low:** measure transfer and render duration; 100 kHz I²C cannot sustain 20 full frames per second.

Do not change remap, COM scan, start line, offsets, or resolution in bulk. Change one variable only after the relevant pattern isolates it.
