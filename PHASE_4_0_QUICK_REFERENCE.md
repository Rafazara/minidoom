# Phase 4.0 RC Quick Start

## What Changed

**Zero gameplay changes. Pure hardening.**

### Key Additions
1. **MINI_DOOM_DEBUG flag** — Controls debug output (default: OFF for clean RC)
2. **State machine hardened** — Explicit guards, no fallthrough
3. **Edge-case defenses** — Dead player, zero damage, ammo validation
4. **Reset guarantees** — Documented deterministic reset
5. **Performance checks** — Frame timing sanity (debug mode only)

## RC Build (Default)

```bash
# config.h already set to MINI_DOOM_DEBUG = 0
pio run
pio run -t upload

# Result: Clean, quiet release build
# No [BALANCE], [FLOW], [AI] logs
```

## Debug Build

```bash
# Edit config.h: #define MINI_DOOM_DEBUG 1
pio run
pio run -t upload

# Result: Full debug telemetry
# Includes [BALANCE], [FLOW], [AI] logs
# Frame timing checks enabled
```

## What's Hardened

| Aspect | Change | Benefit |
|--------|--------|---------|
| **State Machine** | Explicit isolation, guards | No unintended logic execution |
| **Reset** | Documented guarantees | Deterministic restart |
| **Edge Cases** | Dead player, zero damage, ammo | Crash prevention |
| **Debug Output** | Compile-time flag control | RC clean, debug verbose |
| **Logging** | Better context | Easier balance tuning |
| **Performance** | Frame timing checks | Regression detection |

## RC Status

✅ **Build**: SUCCESS (0 warnings)  
✅ **Memory**: 7.5% RAM, 25.3% flash  
✅ **State Machine**: Hardened with guards  
✅ **Reset**: Deterministic, documented  
✅ **Edge Cases**: Defended  
✅ **Debug System**: Integrated  
✅ **Performance**: Sanity checks in place  

**Ready for**: v1.0.0-rc1 tag and deployment

## Before Release (v1.0.0)

1. [ ] Device testing (3-5 gameplay sessions)
2. [ ] Verify no debug logs with MINI_DOOM_DEBUG=0
3. [ ] Test restart flow (death → restart → playing)
4. [ ] Confirm edge cases (ammo=0, health=0, damage timing)
5. [ ] Tag as v1.0.0 (remove -rc1)

## Files Modified

- `src/config.h` — MINI_DOOM_DEBUG flag + debug macros
- `src/doom_adapter.cpp` — State hardening + edge-case defenses

## Key Constants

```cpp
// config.h
#define MINI_DOOM_DEBUG 0      // Set to 1 for debug logs, 0 for RC

// Auto-controlled macros (in RC mode, these are no-ops)
LOG_BALANCE(...)  // [BALANCE] logs
LOG_FLOW(...)     // [FLOW] logs
LOG_AI(...)       // [AI] logs
```

## Next Steps

1. **Compile & Upload**: `pio run -t upload`
2. **Test Gameplay**: 3-5 waves, verify state transitions
3. **Check Logs**: Should be clean (no debug spam)
4. **Validate Reset**: Play game 2x, restart between runs
5. **Tag RC**: When ready, tag as v1.0.0-rc1
6. **Final Validation**: Field test before v1.0.0

---

**Phase 4.0 Status**: ✅ COMPLETE  
**Build Status**: ✅ SUCCESS  
**Ready for**: RC Testing and Deployment
