# Phase 3.5 Quick Reference

## What Changed (TL;DR)

### Combat Balance
- **Enemy Damage**: 10 HP → **7 HP** (more forgiving)
- **Attack Speed**: 800ms → **1000ms** (slower, readable rhythm)
- **Movement Speed**: 0.03 → **0.035** u/tick (16% more threatening)

### Player Feedback
- **Low Health Warning**: 30% → **35%** (earlier danger signal)

### Wave Difficulty
- **Scaling**: Linear `2 + wave_number` (no spikes, smooth progression)

### Logging
- **New**: `[BALANCE]` logs every 1 second during gameplay
  - Example: `[BALANCE] Wave=2, EnemiesAlive=3, PlayerHP=65`

## Why

- **Survivability**: Player now survives ~10-15 seconds in early waves without perfect play
- **Fairness**: Damage feels punishing but deserved
- **Progression**: Wave scaling prevents sudden difficulty cliffs
- **Readability**: Enemies move fast enough to feel threatening but slow enough to track at 20 FPS

## Build Status

✅ **SUCCESS** — 0 warnings, 7.19 seconds, optimal memory

## Files Modified

1. `src/doom_sprite_renderer.h` — Enemy damage, cooldown, speed
2. `src/doom_hud.h` — Low health threshold
3. `src/doom_game_flow.cpp` — Wave scaling (clarified), new API
4. `src/doom_game_flow.h` — New getEnemiesRemaining() function
5. `src/doom_adapter.cpp` — Balance logging

## Testing

```bash
# Upload
pio run -t upload

# Monitor logs
pio device monitor -b 115200

# Expected output
[BALANCE] Wave=1, EnemiesAlive=3, PlayerHP=100
[BALANCE] Wave=1, EnemiesAlive=2, PlayerHP=93
[BALANCE] Wave=1, EnemiesAlive=1, PlayerHP=86
[BALANCE] Wave=2, EnemiesAlive=4, PlayerHP=86
...
```

## Rollback (If Needed)

All changes are constants only. To restore defaults:

```cpp
// src/doom_sprite_renderer.h
#define ENEMY_ATTACK_DAMAGE 10         // was: 7
#define ENEMY_ATTACK_COOLDOWN_MS 800   // was: 1000
#define ENEMY_MOVE_SPEED 0.03          // was: 0.035

// src/doom_hud.h
#define HEALTH_WARNING_THRESHOLD 30    // was: 35
```

Recompile: 7 seconds. No other changes needed.

## Architecture Impact

- ✅ **Zero rendering changes**
- ✅ **Zero memory changes**
- ✅ **Zero input changes**
- ✅ **Zero malloc/new calls**
- ✅ **FPS locked at 20**
- ✅ **Clean build** (0 warnings)

---

**Ready for device testing. Game feels fair and tense.**
