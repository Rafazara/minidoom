# Firmware architecture

## Entry point and modes

`src/main.cpp` selects the normal game or the isolated OLED diagnostic at compile time. Normal firmware calls `DoomIntegration::setup()` and `DoomIntegration::loop()` from Arduino `setup()`/`loop()`.

`src/config.h` defines OLED geometry, GPIO, address, logging, and timing constants. `src/doom_graphics.h` defines the 128×64 game framebuffer and 20 FPS target. Compile-time assertions require the Adafruit display dimensions and game framebuffer dimensions to match.

## Module dependency map

```text
main.cpp
├── DoomIntegration
│   ├── Wire / OLEDUI
│   │   └── Adafruit_SSD1306 + Adafruit_GFX + Adafruit BusIO
│   ├── DoomGraphics
│   │   ├── shared SSD1306 framebuffer helper
│   │   └── OLEDUI::getDisplay()
│   └── DoomAdapter
│       ├── Input
│       ├── DoomWorldRenderer
│       │   ├── DoomNanoLevel helper
│       │   └── wall textures
│       ├── DoomSpriteRenderer
│       │   ├── sprite textures
│       │   ├── combat / enemy AI
│       │   └── feedback / game-feel hooks
│       ├── DoomHUD
│       ├── DoomTitle
│       ├── DoomGameOver
│       ├── DoomFeedback
│       ├── DoomGameFlow
│       └── DoomGameFeel
└── OLED diagnostic path
    ├── OLEDUI
    ├── DoomGraphics
    └── DoomValidationTest
```

`third_party/doom-nano` is a submodule. The active firmware uses its level data and constants; it does not compile the upstream Arduino sketch or upstream display driver.

## Boot flow

1. `main.cpp` starts serial at 115200 baud.
2. `DoomIntegration` starts Wire on GPIO21/22 at 100 kHz.
3. `OLEDUI` initializes a 128×64 Adafruit SSD1306 object at `0x3C`.
4. The display uses rotation 0 and is cleared.
5. `DoomGraphics::initialize()` clears the composite framebuffer and resets graphics metrics.
6. `DoomAdapter::initialize()` initializes world, sprites, HUD, screens, feedback, waves, game feel, player state, level pointer, and input.
7. The initial state is `TITLE`.

OLED failure is logged. The blit function also refuses to transfer when OLED initialization did not succeed or the library buffer is unavailable.

## Rendering pipeline

```text
Input::readInput
       |
       v
state-specific logic
       |
       +-- TITLE ------> DoomTitle -------+
       |                                  |
       +-- GAME_OVER --> DoomGameOver ----+--> DoomGraphics framebuffer
       |                                  |         (1024 bytes)
       +-- PLAYING                       |
               |                         |
               +--> world raycaster -----+--> world framebuffer (1024 bytes)
               |       + depth buffer    |         + depth (128 bytes)
               +--> copy world           |
               +--> sprites              |
               +--> HUD                  |
               +--> feedback overlays ---+
                                          |
                                          v
                              exact copy to Adafruit buffer
                                          |
                                          v
                                    Wire transactions
                                          |
                                          v
                                       SSD1306
```

All active project renderers use SSD1306 native page layout:

```text
byte index = (y / 8) * 128 + x
bit mask   = 1 << (y & 7)
```

The least-significant bit is the top pixel within each eight-pixel page. `drawBitmap()` is not used for the game buffer because that API expects row-major bitmap input. Rotation is not applied to the raw copied bytes.

## State machine

| State | Behavior | Exit |
|---|---|---|
| `TITLE` | Render and flush title; no world/combat logic | Fire edge enters `PLAYING`; optional validation builds may auto-transition |
| `PLAYING` | Movement, AI, combat, world, sprites, HUD, effects | Health zero enters `GAME_OVER` |
| `GAME_OVER` | Render and flush game-over screen; no gameplay logic | Fire edge resets state and enters `PLAYING` |

The default environment no longer defines `VALIDATION_MODE`, so it requires Fire rather than auto-starting after three seconds. Reset restores player, ammunition, combat/damage transients, sprites, and wave-flow state.

## Memory ownership

- `DoomGraphics`: static 1,024-byte composite framebuffer.
- `DoomWorldRenderer`: static 1,024-byte world framebuffer and 128-byte depth buffer.
- `DoomSpriteRenderer`: static array of four sprite records; a small fixed-size render-job array is stack allocated.
- `Adafruit_SSD1306`: allocates its own 1,024-byte display buffer during `begin()` for the corrected 128×64 configuration.
- Level and texture arrays: static const data, retained in flash on ESP32.
- Project code introduces no `malloc`, `calloc`, `realloc`, or `new` calls. This does not mean the firmware has zero heap use: Adafruit and Arduino/framework code allocate internally.

The two project framebuffers serve different stages and avoid aliasing the world/depth pass with final overlays. No buffer consolidation was attempted during recovery.

## Timing and logging

Both integration and adapter use 50 ms pacing thresholds. Rendering and full-screen I²C transfer occur synchronously. The display is transferred through Adafruit SSD1306 in multiple Wire transactions.

At 100 kHz, 1,024 payload bytes plus control/address/ACK overhead take roughly 90 ms or more. Therefore the configured 20 FPS target is not physically credible at this clock without measurement and a faster validated bus setting. Compilation does not validate timing.

Many legacy modules also contain direct `Serial` logging, including render-path messages. `MINI_DOOM_DEBUG=0` does not suppress all of them. This overhead remains a release-readiness issue and must be measured or cleaned up in a separately reviewed change, including protected modules where applicable.
