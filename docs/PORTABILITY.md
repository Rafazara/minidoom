# AVR-to-ESP32 portability

## Confirmed failure

`src/doom_adapter.cpp` includes the Doom Nano `level.h`. The upstream header unconditionally included:

```cpp
#include <avr/pgmspace.h>
```

ESP32 Arduino does not provide that AVR path, so compilation stopped before firmware linking. The failure was reproduced before the fix.

## Selected solution

The project uses a minimal include-path wrapper plus the framework's own implementation:

```text
third_party/doom-nano/level.h
    -> <avr/pgmspace.h>
    -> include/avr/pgmspace.h
    -> include/pgmspace_compat.h
    -> ESP32 Arduino <pgmspace.h>
```

On AVR, the compatibility header selects the real AVR header. On non-AVR Arduino targets, it selects the target's `<pgmspace.h>`.

This is a combination of framework-provided compatibility and a narrow portability abstraction. It does not redefine `pgm_read_byte`, pointer reads, string functions, or memory-copy functions.

## Why it is correct on ESP32

ESP32 has a unified address space for the const data used here. The installed ESP32 Arduino core defines:

- `PROGMEM` as an empty annotation.
- `pgm_read_byte` and related reads as native memory accesses.
- `PSTR`, `memcpy_P`, `strlen_P`, and related functions as their native equivalents.

The firmware therefore keeps assets as static const data without copying level or texture assets into RAM. Using the core's macros preserves its pointer and data-access semantics rather than inventing substitutes.

No active project use of `pgm_read_word` or `pgm_read_dword` was found. Texture pointer tables use the core's `pgm_read_ptr`; normal pointer alignment is preserved. Unused upstream AVR display and sound code is not compiled into the firmware.

## Third-party preservation

The solution does not modify the Doom Nano submodule. Upstream `level.h`, `sprites.h`, and `sound.h` can retain their original AVR includes. The wrapper handles all three without a bulk search-and-replace.

The superproject records Doom Nano from <https://github.com/daveruiz/doom-nano.git> at submodule revision `234640478b38f16558b34b543ef6ff14b72e3fb2`. No local third-party source patch is part of this recovery.

The original MINI-DOOM project code uses the MIT License already present on the repository's default branch; `LICENSE` is preserved unchanged in this recovery tree. The upstream Doom Nano snapshot contains no repository-wide license file. Individual bundled display/TWI sources retain their own notices, and external sprite rights are not established. The project license does not override these third-party limitations.

## Shared map decoder

`include/doom_nano_level.h` implements the upstream map representation used by both rendering and collision:

- 64×57 cells.
- Two 4-bit cells per byte.
- Rows stored bottom-up.
- Even columns in the low nibble; odd columns in the high nibble.
- Out-of-range/null access returns a wall.
- AVR uses `pgm_read_byte`; non-AVR targets use a direct const-byte read.

Sharing this helper prevents the previous disagreement where the world renderer used top-down rows and reversed nibbles while movement used another convention. The protected renderer change was explicitly approved before editing.

## Review guidance

When adding another AVR-derived source file:

1. Identify whether it is actually compiled; do not port unused upstream drivers by default.
2. Prefer the existing wrapper and target framework macros.
3. Check data size, integer width, pointer width, alignment, and endian assumptions.
4. Keep flash assets const; do not duplicate them into RAM without evidence.
5. Add a focused host test when the representation is architecture-independent.
6. Preserve upstream attribution and document any future submodule patch.
