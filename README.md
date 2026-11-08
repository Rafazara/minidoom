# MINI-DOOM

A tiny Doom-inspired embedded game for ESP32 and monochrome OLED displays.

MINI-DOOM combines a raycasting world, sprites, combat, a HUD, and title/game-over screens on a 128×64 display. It uses selected Doom Nano data and ideas; it is **not a port of the original Doom engine**.

## Status

**Development baseline — software build recovered, hardware validation pending.**

- The `esp32doit-devkit-v1` firmware and isolated OLED diagnostic both compile.
- Host test sources cover framebuffer packing and Doom Nano map decoding; the current Windows host lacks the GCC tools needed to run them.
- No firmware from this recovery work has been uploaded or visually validated on the OLED.
- A 20 FPS target is configured, but physical frame rate is not validated. The conservative 100 kHz I²C setting cannot transfer a full 1,024-byte frame within 50 ms.

See [release status](docs/RELEASE_STATUS.md) for the current decision and blockers.

## Capabilities

Source inspection confirms implementations for:

- 128×64, 1-bit SSD1306 page-format rendering.
- Raycast walls with textures and ordered dithering.
- Enemy and weapon sprites with a per-column depth buffer.
- Movement, collision, combat, health, ammunition, and wave logic.
- Title, gameplay, and game-over states.
- Compile-time game and isolated OLED diagnostic environments.

These are implementation capabilities, not a claim that every feature has passed hardware testing.

## Hardware baseline

| Component | Validated configuration |
|---|---|
| MCU board | ESP32 DOIT DevKit V1; ESP32-D0WDQ6 observed during previous uploads |
| Flash | 4 MB reported by PlatformIO |
| Display | 128×64 SSD1306-compatible I²C OLED |
| OLED address | `0x3C`, confirmed by a previous physical I²C scan |
| I²C data | GPIO21 |
| I²C clock | GPIO22 |
| Power | OLED VCC at 3.3 V, common GND |
| USB serial | COM7 previously used; 115200 baud monitor |

Only the ESP32 and OLED are part of the confirmed baseline. Buttons, speakers, batteries, and other peripherals are optional future work, not assumed to be installed.

## Wiring

| OLED pin | ESP32 connection |
|---|---|
| GND | GND |
| VCC | 3.3V |
| SCL / SCK | GPIO22 |
| SDA | GPIO21 |

```text
ESP32 DOIT DevKit V1                 OLED module
+-------------------+                +-----------+
| 3.3V              |--------------->| VCC       |
| GND               |--------------->| GND       |
| GPIO22 (SCL)      |--------------->| SCL/SCK   |
| GPIO21 (SDA)      |--------------->| SDA       |
| USB               |<-- data/power--| Host PC   |
+-------------------+                +-----------+
```

Power down before changing wiring. ESP32 GPIO uses 3.3 V logic. See [the hardware guide](docs/HARDWARE.md) for power and safety notes.

## Quick start

### Prerequisites

- PlatformIO Core available as `pio`.
- Git with submodule support.
- ESP32 board and OLED wired as above.
- A USB data cable.

### Clone and build

```powershell
git clone --recurse-submodules https://github.com/Rafazara/minidoom.git
cd minidoom
git submodule update --init --recursive
pio run -e esp32doit-devkit-v1
```

The configuration pins Espressif32 6.12.0, Adafruit GFX 1.13.0, and Adafruit SSD1306 2.5.17 to limit dependency drift.

### Upload and monitor

Check `upload_port` and `monitor_port` in `platformio.ini`; COM7 is a previous local setting, not a universal value. Close other serial-monitor sessions before uploading.

```powershell
pio run -e esp32doit-devkit-v1 -t upload
pio device monitor -p COM7 -b 115200
```

Do not interpret a successful upload as a display-validation pass. Record the actual OLED behavior and serial output.

More detail: [Build and flash](docs/BUILD_AND_FLASH.md).

## Controls

The firmware contains optional active-low button mappings using internal pull-ups:

| Action | GPIO |
|---|---|
| Forward | 32 |
| Backward | 33 |
| Strafe left | 25 |
| Strafe right | 26 |
| Fire / start / restart | 27 |

No buttons are confirmed connected in the current hardware baseline. Without a Fire button, the normal game build remains on the title screen. Wiring and debounce behavior require physical validation before controls are described as working.

## Architecture

```text
Input GPIO
    |
    v
DoomAdapter: state, movement, combat, reset
    |
    +--> DoomWorldRenderer --> world framebuffer + depth buffer
    |
    +--> Sprite renderer --> composite framebuffer
    +--> HUD / title / game-over / feedback overlays
    |
    v
DoomGraphics: 128x64 page-format framebuffer
    |
    |  exact 1,024-byte copy; no bitmap reformatting
    v
Adafruit_SSD1306 heap buffer --> Wire / I2C --> OLED
```

There are two project-owned 1,024-byte framebuffers: one for the world renderer and one for final composition. Adafruit SSD1306 allocates its own display buffer during `begin()`.

See [Architecture](docs/ARCHITECTURE.md) and [Portability](docs/PORTABILITY.md).

## Memory and performance

Measured for the recovered default build on this host:

| Metric | Result |
|---|---:|
| Static RAM reported by PlatformIO | 24,516 / 327,680 bytes (7.5%) |
| Flash | 329,185 / 1,310,720 bytes (25.1%) |
| Target frame rate | 20 FPS / 50 ms |
| I²C clock | 100 kHz |

PlatformIO's RAM figure does not include the display buffer allocated at runtime by Adafruit SSD1306. At 100 kHz, a full-frame transfer alone takes roughly 90 ms including protocol overhead, so 20 FPS requires hardware measurement and likely a validated faster I²C configuration. No physical FPS claim is made.

## OLED diagnostic mode

The isolated diagnostic does not run game logic. It cycles through blank/filled screens, coordinates, lines, checkerboard, border, distinct corner markers, text orientation, and a moving square.

After obtaining approval to flash the device:

```powershell
pio run -e esp32doit-devkit-v1-oled-diag -t upload
pio device monitor -p COM7 -b 115200
```

Return to the game by uploading the default environment:

```powershell
pio run -e esp32doit-devkit-v1 -t upload
```

No source edit is needed to switch environments. Follow the observation checklist in [OLED diagnostics](docs/OLED_DIAGNOSTICS.md).

## Troubleshooting

- **`avr/pgmspace.h` missing:** build the current recovery tree; the compatibility wrapper must be under `include/avr/pgmspace.h` and the submodule must be initialized.
- **OLED not detected:** power down, verify 3.3 V/GND and SDA/SCL, then run an I²C scan. Address detection alone does not prove controller compatibility.
- **Horizontal bands or fragmented graphics:** run the isolated diagnostic and record blank, filled, border, and corner results. Do not start by changing rotation.
- **Blank title in normal mode:** verify that the firmware includes the title flush fix and that no I²C initialization error appears.
- **Upload failed:** confirm the actual COM port and close serial-monitor programs.
- **Native tests do not build:** install or expose a supported GCC/G++ host toolchain; the current host does not provide one.

## Development roadmap

1. Run the OLED diagnostic on the physical module and capture each required result.
2. Run the host tests with a supported native compiler.
3. Validate normal title, gameplay, game-over, and reset behavior on hardware.
4. Measure frame time and heap during gameplay; evaluate I²C speed only after the display is stable.
5. Resolve project and upstream asset licensing before a public release.
6. Prepare a release candidate only after the acceptance criteria pass.

## Documentation

- [Hardware and wiring](docs/HARDWARE.md)
- [Build and flash](docs/BUILD_AND_FLASH.md)
- [Architecture](docs/ARCHITECTURE.md)
- [AVR-to-ESP32 portability](docs/PORTABILITY.md)
- [OLED diagnostics](docs/OLED_DIAGNOSTICS.md)
- [Testing and acceptance](docs/TESTING.md)
- [Release status](docs/RELEASE_STATUS.md)

Root-level phase reports are retained as historical development records. The guides above describe the current maintained baseline.

## Credits and licensing

- [Doom Nano](https://github.com/daveruiz/doom-nano) by David Ruiz and contributors, included as a Git submodule at the pinned upstream revision.
- Doom Nano credits raycasting learning material to [lodev.org](https://lodev.org/cgtutor) and identifies external sprite sources in its README.
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library), [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306), and Adafruit BusIO provide display support.
- ESP32 Arduino and PlatformIO provide the framework and build system.

Third-party notices in the submodule are preserved. The upstream Doom Nano snapshot has no repository-wide license file, and its external sprite permissions are not established by this project. No project-wide license file currently exists for MINI-DOOM; original project code is therefore not granted an open-source license by this README. License selection and asset-rights review remain release blockers.

### Project media needed

No genuine MINI-DOOM hardware screenshot is currently included. For a future visual update, capture a straight-on photo of the actual ESP32/OLED wiring and clear photos of the diagnostic border, title screen, and gameplay. Do not substitute upstream Arduino screenshots as images of this hardware.
