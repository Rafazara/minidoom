#ifndef DOOM_GAME_FLOW_H
#define DOOM_GAME_FLOW_H

#include <stdint.h>

/**
 * Doom Game Flow - Phase 2.15
 *
 * Simple wave progression system:
 * - Waves with increasing enemy count
 * - Breathing room between waves
 * - Minimal overlay text ("WAVE N", "CLEAR")
 *
 * No dynamic allocation; O(1) update and lightweight render.
 */

namespace DoomGameFlow {

void initialize();

// Called every frame during GAME_STATE_PLAYING
void update(uint32_t current_time_ms);

// Called when an enemy dies
void notifyEnemyKilled();

// Render overlay (after HUD + feedback, before blit)
void render(uint8_t* framebuffer, uint32_t current_time_ms);

// Used by adapter
bool shouldSpawnWave();
uint8_t getCurrentWave();
uint8_t getEnemyCountForWave();
uint8_t getEnemiesRemaining();  // Phase 3.5: For balance logging

}  // namespace DoomGameFlow

#endif  // DOOM_GAME_FLOW_H

