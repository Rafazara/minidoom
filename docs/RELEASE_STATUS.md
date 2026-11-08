# Release status

## Decision: PARTIAL

The ESP32 build and OLED diagnostic path are validated. The user confirmed that the corrected OLED diagnostic works on physical hardware. Full gameplay, controls, reset, timing, and stability remain unvalidated, so this tree is not a final release.

## Completed with evidence

- Reproduced and corrected the missing `avr/pgmspace.h` failure without changing the Doom Nano submodule.
- Built the normal and OLED diagnostic environments successfully on ESP32.
- Corrected the confirmed 128×32/128×64 mismatch and resulting 512-byte overwrite.
- Unified framebuffer representation and orientation behavior.
- Added graphics initialization and title/game-over flushes.
- Aligned world and collision map decoding through a bounded shared helper; the protected renderer edit was explicitly approved.
- Added compile-time buffer-dimension assertions, isolated display patterns, and host-test sources.
- Received user confirmation that the corrected OLED diagnostic works on the physical device.
- Preserved the existing MIT License for original MINI-DOOM project code.

## Validation boundaries

| Scope | Status |
|---|---|
| ESP32 main compilation | PASS |
| ESP32 diagnostic compilation | PASS |
| OLED diagnostic on hardware | PASS — user-confirmed general result |
| Pattern-by-pattern visual checklist | NOT ARCHIVED |
| Gameplay and controls on hardware | PENDING |
| Game-over, waves, and reset on hardware | PENDING |
| Native tests on the Windows recovery host | BLOCKED — GCC/G++ unavailable |
| Native tests and builds in CI | Reported by the GitHub Actions PR check |
| 20 FPS and runtime stability | NOT MEASURED |

## Unresolved blockers and risks

1. **Gameplay validation is incomplete.** The OLED diagnostic does not validate input, movement, combat, waves, game-over, or reset.
2. **20 FPS is only a target.** Full-frame transfer at 100 kHz takes roughly 90 ms or more, so the target is not achievable at that bus speed.
3. **Release logging remains noisy.** Legacy direct `Serial` calls bypass `MINI_DOOM_DEBUG`, including render-path logging.
4. **Third-party rights need review.** Doom Nano has no repository-wide license file in the pinned snapshot, and external sprite permissions are unresolved. The project MIT License does not remove third-party obligations.
5. **Controls are not a validated baseline.** Firmware has optional GPIO mappings, but no buttons are confirmed connected.

## Environment limitations

PlatformIO Core 6.1.18 on Python 3.15.0a2 emits an obsolete-core notice and a `requests` dependency warning. Both firmware builds nevertheless succeed. No global environment changes were made. CI uses a supported Python version and an Ubuntu native toolchain.

## Git delivery scope

Work uses the existing `minidoom-oled-test` worktree on local branch `test/minidoom-oled`, tracking `origin/fix/mini-doom-framebuffer-safety`. No branch is deleted, reset, rewritten, or force-pushed. The separate `master` worktree and its pre-existing staged submodule edit remain untouched.

Publication and pull-request integration are authorized. The remote default branch is `main`; its unrelated history must be handled by the GitHub PR workflow without forcing or rewriting either line of development.

## Promotion criteria

Do not promote this tree to a final release until:

- automated tests execute and pass locally or in CI;
- normal gameplay, controls, game-over, waves, and reset pass on hardware;
- a stability run and actual timing/heap measurements are recorded;
- release logging is controlled;
- third-party licensing and sprite rights are addressed.

The next physical action is to validate the normal TITLE screen and, with confirmed controls, one complete start → play → game-over → restart cycle.
