/**
 * Doom Sprite Renderer - Phase 2.9 (Multiple Enemies)
 * 
 * Purpose:
 * Renders 2D sprites projected into 3D raycasted world
 * Handles depth-based occlusion (sprites behind walls)
 * Supports player weapon and multiple enemy sprites simultaneously
 * 
 * Architecture:
 * - Static sprite list (max 4: 1 weapon + 3 enemies, or 1 weapon + 4 enemies)
 * - Per-column depth buffer (from raycaster)
 * - Distance-based scaling and projection
 * - Depth-sorted rendering (back-to-front)
 * - Monochrome dithering for 1-bit OLED
 * 
 * Phase 2.9 Enhancements:
 * - Multiple simultaneous enemies with unique IDs
 * - Depth-sorted rendering for correct occlusion
 * - spawnEnemies() for initial enemy placement
 * - Improved hit detection for closest enemy
 * - Per-enemy logging with ID tracking
 * 
 * Integration Point:
 * Called AFTER renderWorld() to overlay sprites with correct depth
 * Receives depth buffer from raycaster for occlusion
 * 
 * Memory Usage:
 * - 4 sprites × 20 bytes = 80 bytes (struct data)
 * - Texture data in PROGMEM (not SRAM)
 * - Total SRAM overhead: ~100 bytes
 */

#ifndef DOOM_SPRITE_RENDERER_H
#define DOOM_SPRITE_RENDERER_H

#include <stdint.h>
#include <string.h>

namespace DoomSpriteRenderer {

// ============================================================================
// SPRITE TYPES & CONSTANTS
// ============================================================================

// Sprite type IDs
enum SpriteType {
    SPRITE_WEAPON = 0,   // Player weapon (always rendered at bottom)
    SPRITE_ENEMY = 1     // Enemy sprite
};

// Sprite rendering states
enum SpriteState {
    SPRITE_INACTIVE = 0,
    SPRITE_ACTIVE = 1
};

// Enemy combat states
enum EnemyState {
    ENEMY_ALIVE = 0,
    ENEMY_HIT = 1,       // Hit but not yet dead (visual feedback phase)
    ENEMY_DEAD = 2
};

// Enemy AI states (Phase 2.10)
enum EnemyAIState {
    AI_IDLE = 0,         // Standing still
    AI_CHASE = 1,        // Moving toward player
    AI_ATTACK = 2,       // Attacking player
    AI_HIT = 3,          // Stunned from being shot
    AI_DEAD = 4
};

// Enemy hit feedback constants
#define ENEMY_HIT_DURATION_FRAMES 3  // Duration of hit state before death
#define ENEMY_HIT_OFFSET_Y 2         // Vertical offset when hit (pixels)

// Enemy AI constants (Phase 2.10)
#define ENEMY_AWARENESS_DISTANCE 20.0  // Distance at which enemy detects player
#define ENEMY_MOVE_SPEED 0.035        // Units per tick (Phase 3.5: tuned for pressure/readability balance)
#define ENEMY_ATTACK_DISTANCE 2.0      // Distance to trigger attack
#define ENEMY_ATTACK_COOLDOWN_MS 1000  // ms between attacks (Phase 3.5: 1000ms gives player 10-15s survival window)
#define ENEMY_ATTACK_DAMAGE 7          // Damage per hit (Phase 3.5: 7 HP = ~14 hits to kill at 100 HP, feels punishing but fair)

// Enemy spawn constants (Phase 2.9)
#define INITIAL_ENEMY_COUNT 2        // Initial number of enemies to spawn
#define ENEMY_SPAWN_DISTANCE 20.0    // Starting distance from player
#define ENEMY_SPAWN_ANGLE_SPREAD 60  // Angle spread for initial placement (degrees)

// Texture dimensions (in pixels)
#define SPRITE_WIDTH    16
#define SPRITE_HEIGHT   16
#define SPRITE_PIXELS   (SPRITE_WIDTH * SPRITE_HEIGHT)
#define SPRITE_BYTES    (SPRITE_PIXELS / 8)  // 1-bit = 32 bytes per row × 16 rows = need to adjust

// Combat constants
#define COMBAT_HIT_DISTANCE_THRESHOLD 50.0  // Max distance to hit enemy
#define COMBAT_HIT_ANGLE_THRESHOLD 2        // ±2 screen columns for hit

// ============================================================================
// SPRITE STRUCTURE
// ============================================================================

/**
 * Sprite definition - 28 bytes (Phase 2.10 extended with AI data)
 * Stores position, texture, state, and AI behavior data
 */
struct Sprite {
    double x;                     // World X position
    double y;                     // World Y position
    uint8_t type;                 // SPRITE_WEAPON or SPRITE_ENEMY
    uint8_t texture_id;           // Texture reference
    uint8_t active;               // SPRITE_ACTIVE or SPRITE_INACTIVE
    uint8_t enemy_state;          // ALIVE, HIT, DEAD
    uint8_t hit_frame_counter;    // Frames in HIT state
    uint8_t enemy_id;             // Unique enemy ID
    uint8_t ai_state;             // AI_IDLE, AI_CHASE, AI_ATTACK, etc (Phase 2.10)
    uint16_t attack_cooldown_ms;  // Cooldown timer for attacks (Phase 2.10)
    uint8_t padding[1];           // Alignment padding
};  // Total: 28 bytes

// ============================================================================
// MODULE INTERFACE
// ============================================================================

/**
 * Initialize sprite rendering system
 * Sets up sprite list and texture references
 * Called once during startup
 */
void initialize();

/**
 * Add sprite to active list
 * @param sprite: Sprite definition
 * @return true if added, false if list full
 */
bool addSprite(const Sprite& sprite);

/**
 * Remove sprite from active list
 * @param index: Sprite list index (0-3)
 */
void removeSprite(uint8_t index);

/**
 * Clear all active sprites
 */
void clearSprites();

/**
 * Render all active sprites with depth-based occlusion
 * Called after renderWorld() to overlay sprites
 * 
 * @param framebuffer: Target 1024-byte monochrome framebuffer (128×64)
 * @param depth_buffer: Per-column depth buffer from raycaster (128 bytes)
 * @param player_x: Player world X position (for distance calculation)
 * @param player_y: Player world Y position (for distance calculation)
 * @param player_dir_x: Player direction X component
 * @param player_dir_y: Player direction Y component
 * @param player_plane_x: Camera plane X component
 * @param player_plane_y: Camera plane Y component
 * 
 * Logic:
 * - Project each sprite to screen space
 * - Check depth against raycasted walls
 * - Render only visible parts (clipped at wall depth)
 * - Apply monochrome dithering
 */
void renderSprites(
    uint8_t* framebuffer,
    const uint8_t* depth_buffer,
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    double player_plane_x,
    double player_plane_y
);

/**
 * Render player weapon (fixed screen position)
 * Called as special case during renderSprites()
 * Weapon always at bottom-center, never occluded
 */
void renderWeapon(uint8_t* framebuffer);

/**
 * Check if sprite is visible (not fully occluded)
 * @param sprite_dist: Distance from player to sprite
 * @param screen_x: Screen column position
 * @param depth: Depth value at that screen column
 * @return true if sprite should be rendered at this position
 */
bool isSpriteVisible(double sprite_dist, uint8_t screen_x, uint8_t depth);

/**
 * Get sprite at index
 * @param index: Sprite list index (0-3)
 * @return Pointer to sprite data, nullptr if index invalid
 */
const Sprite* getSprite(uint8_t index);

/**
 * Get active sprite count
 * @return Number of sprites in active list
 */
uint8_t getSpriteCount();

/**
 * Check initialization status
 * @return true if module initialized
 */
bool isInitialized();

// ============================================================================
// COMBAT FUNCTIONS
// ============================================================================
/**
 * Spawn initial enemies (Phase 2.9)
 * Creates INITIAL_ENEMY_COUNT enemies at spawn positions
 * 
 * @param player_x: Player world X position (for relative placement)
 * @param player_y: Player world Y position (for relative placement)
 * @param player_dir_x: Player direction X (for angle calculation)
 * @param player_dir_y: Player direction Y (for angle calculation)
 * 
 * Behavior:
 * - Creates 2-4 enemies in formation around player
 * - Each enemy placed at ENEMY_SPAWN_DISTANCE units away
 * - Spread across ENEMY_SPAWN_ANGLE_SPREAD degrees
 * - All initialized with ENEMY_ALIVE state
 * - All active and ready for rendering
 * - Logs: [ENEMY] <id> spawned at (X, Y)
 */
void spawnEnemies(
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y
);

/**
 * Spawn enemies with explicit count (Phase 2.15)
 * Same behavior as spawnEnemies(), but caller can request a specific count.
 * If requested_count is 0, falls back to INITIAL_ENEMY_COUNT.
 */
void spawnEnemies(
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    uint8_t requested_count
);

/**
 * Check if central ray hits any enemy
 * Performs hit detection on center screen ray (x=64) with ±threshold columns
 * 
 * @param depth_buffer: Per-column depth buffer from raycaster
 * @param player_x: Player world X position
 * @param player_y: Player world Y position
 * @param player_dir_x: Player direction X
 * @param player_dir_y: Player direction Y
 * @param player_plane_x: Camera plane X
 * @param player_plane_y: Camera plane Y
 * @param hit_distance: Output parameter - distance to hit enemy (if hit)
 * @return Index of hit enemy sprite (0-3), or 255 if no hit
 * 
 * Algorithm:
 * - Cast central ray at screen_x = 64
 * - Check depth columns [62, 66] for enemy sprites
 * - Compare sprite distance vs depth_buffer[col]
 * - Return first hit (closest sprite)
 */
uint8_t checkHitDetection(
    const uint8_t* depth_buffer,
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    double player_plane_x,
    double player_plane_y,
    double& hit_distance
);

/**
 * Update enemy AI state machine (Phase 2.10)
 * Called each game tick to advance enemy behavior
 * 
 * State Transitions:
 * - AI_IDLE: Waits for player within ENEMY_AWARENESS_DISTANCE
 * - AI_CHASE: Moves toward player at ENEMY_MOVE_SPEED
 * - AI_ATTACK: Attacks player when within ENEMY_ATTACK_DISTANCE with cooldown
 * - AI_HIT: Handled by renderSprites() - transitions back to CHASE when recovered
 * - AI_DEAD: No action
 * 
 * @param player_x: Player world X position
 * @param player_y: Player world Y position
 * @param current_time_ms: Current system time in milliseconds (for cooldown tracking)
 * 
 * Implementation:
 * - Updates each active enemy's ai_state based on distance to player
 * - Moves enemies toward player in CHASE state
 * - Applies damage to player in ATTACK state when cooldown expired
 * - Logs [AI] state transitions for diagnostics
 * 
 * Performance: <2ms per frame (all enemies combined)
 */
void updateEnemyAI(
    double player_x,
    double player_y,
    uint32_t current_time_ms
);

/**
 * Handle combat resolution
 * Called each game tick to process shooting and hit detection
 * 
 * @param fire_pressed: true if FIRE button is currently pressed
 * @param depth_buffer: Per-column depth buffer from raycaster
 * @param player_x: Player world X position
 * @param player_y: Player world Y position
 * @param player_dir_x: Player direction X
 * @param player_dir_y: Player direction Y
 * @param player_plane_x: Camera plane X
 * @param player_plane_y: Camera plane Y
 * 
 * Effects:
 * - Logs [COMBAT] FIRE when fire_pressed and not already fired this tick
 * - Checks for hit using checkHitDetection()
 * - Logs [COMBAT] HIT enemy at dist=X.Y on hit
 * - Marks hit enemy as ENEMY_DEAD
 * - Logs [COMBAT] ENEMY DOWN after removal
 * - Sets enemy active=false to stop rendering
 */
void handleCombat(
    bool fire_pressed,
    const uint8_t* depth_buffer,
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    double player_plane_x,
    double player_plane_y
);

}  // namespace DoomSpriteRenderer

#endif // DOOM_SPRITE_RENDERER_H
