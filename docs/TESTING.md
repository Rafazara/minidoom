# Testing

## Automated firmware builds

Run both deliverable environments:

```powershell
pio run -e esp32doit-devkit-v1
pio run -e esp32doit-devkit-v1-oled-diag
```

Recovery results on the current host:

| Environment | Status | RAM | Flash |
|---|---|---:|---:|
| `esp32doit-devkit-v1` | SUCCESS | 24,516 bytes, 7.5% | 329,185 bytes, 25.1% |
| `esp32doit-devkit-v1-oled-diag` | SUCCESS | 23,008 bytes, 7.0% | 305,977 bytes, 23.3% |

No compiler diagnostics were emitted in these builds. PlatformIO core and Python dependency warnings are environmental and are listed separately in the build guide.

The default build includes compile-time assertions for display/game dimensions. The OLED correction also restores the library's 128×64 initialization and 1,024-byte runtime allocation.

## Host-side tests

Test sources:

- `test/test_framebuffer/test_framebuffer.cpp`
  - page offsets for all edges;
  - LSB-first bit order;
  - set/get/clear behavior;
  - null and out-of-range coordinates;
  - guard bytes around the 1,024-byte buffer.
- `test/test_level_decoding/test_level_decoding.cpp`
  - low/high nibble selection;
  - bottom-up row indexing;
  - fractional coordinates;
  - negative/out-of-range/null handling.

Run:

```powershell
pio test -e native
```

Current status: **BLOCKED BY HOST TOOLCHAIN**. PlatformIO collected both test suites, but `gcc` and `g++` are not installed or not on PATH. No assertions executed, so no automated test PASS is claimed.

## Static inspection completed

- Entry point, boot order, OLED address and dimensions.
- Project-owned framebuffer sizes and ownership.
- Pixel offsets and bit packing in world, sprites, HUD, title, game over, feedback, flow, and game feel.
- Final copy into Adafruit's buffer and `display()` transfer.
- State isolation and title/game-over flush behavior.
- Static allocation and project absence of direct dynamic allocation.
- Doom Nano PROGMEM compatibility and map representation.
- Reset of player, ammunition, effects, sprites, and wave state.

Static inspection cannot prove the absence of all runtime defects. Adafruit's heap allocation and framework allocations are outside PlatformIO's static RAM figure.

## Physical acceptance procedure

Physical tests require explicit approval before an agent uploads firmware.

1. Record module markings/photos, wiring, commit, and environment.
2. Build and upload the isolated diagnostic.
3. Record every pattern using the template in `OLED_DIAGNOSTICS.md`.
4. Require blank, filled, border, corner, text, and moving-shape results before judging the game.
5. Upload normal firmware and verify title visibility.
6. With confirmed optional controls, verify start, movement, combat, death, restart, and reset.
7. Run for at least 30 seconds, then a longer soak if stable; record resets and I²C errors.
8. Measure actual frame duration, FPS, free heap, and minimum free heap. Do not infer them from compilation.

## Release acceptance criteria

A release candidate requires:

- Reproducible default and diagnostic builds with no compiler errors.
- Executed host tests passing.
- Physical OLED geometry/orientation/stability passing.
- Normal state transitions and reset passing on hardware with the intended controls.
- Measured memory and timing documented; any FPS deviation explicitly accepted or corrected.
- No uncontrolled release-path debug spam.
- Third-party attribution preserved and licensing/asset rights resolved.

A successful build alone is never a hardware or release PASS.
