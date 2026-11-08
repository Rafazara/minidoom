# Release status

## Decision: PARTIAL

Software build recovery and focused firmware corrections are implemented. Physical validation is pending, and the host test runner is blocked by the absence of a native GCC/G++ toolchain. This tree is not yet a validated release candidate.

## Completed with evidence

- Reproduced the missing `avr/pgmspace.h` failure on the recovery branch.
- Added a narrow wrapper that delegates to ESP32 Arduino's native `<pgmspace.h>` implementation.
- Preserved the Doom Nano submodule without local source modifications.
- Built the normal and OLED diagnostic environments successfully.
- Corrected the confirmed 128×32/128×64 buffer mismatch and resulting 512-byte overwrite.
- Unified raw framebuffer and GFX orientation behavior.
- Added missing graphics initialization and title/game-over flushes.
- Aligned world and collision map decoding through a bounded shared helper; the protected renderer edit was explicitly approved.
- Added compile-time buffer-dimension assertions.
- Added isolated display patterns and host-test sources.
- Preserved gameplay constants, renderer algorithms, input mappings, and 20 FPS target.

## Unresolved blockers and risks

1. **OLED hardware not validated.** No recovery firmware has been uploaded or observed. The current panel/controller identity is not established.
2. **Host tests not executed.** PlatformIO discovered them but could not find `gcc` or `g++`.
3. **20 FPS not validated.** Full-frame transfer at 100 kHz takes roughly 90 ms or more, so the target is not achievable at that bus speed. No timing change was made without hardware evidence.
4. **Release logging remains noisy.** Legacy direct `Serial` calls bypass `MINI_DOOM_DEBUG`, including render-path logging. A focused, separately reviewed cleanup is still required.
5. **Licensing is incomplete.** No MINI-DOOM project-wide license or upstream Doom Nano repository-wide license file is present; external sprite rights are unresolved.
6. **Controls are not a validated baseline.** Firmware has optional GPIO mappings, but no buttons are confirmed connected.

## Environment limitations

PlatformIO Core 6.1.18 on Python 3.15.0a2 emits an obsolete-core notice and a `requests` dependency warning. Both firmware builds nevertheless succeed. No global environment changes were made.

## Git delivery scope

Work is confined to the existing `minidoom-oled-test` worktree on local branch `test/minidoom-oled`, which tracks `origin/fix/mini-doom-framebuffer-safety`. No branch was deleted, reset, merged, or force-pushed. The separate `master` worktree and its pre-existing staged submodule edit were not modified.

Remote publishing and merging require explicit approval.

## Promotion criteria

Do not promote this tree to a release candidate until:

- host tests execute and pass;
- the OLED diagnostic and normal game pass the physical checklist;
- controls/reset and a stability run are verified on the intended hardware;
- actual frame timing and runtime heap are recorded;
- release logging is controlled;
- licensing and asset rights are addressed.

The next physical action is to run the isolated OLED diagnostic and record blank, filled, border, corner, text, and moving-square behavior.
