/**
 * Doom Sprite Renderer - Phase 2.5 Implementation
 * 
 * Renders 2D sprites projected into 3D raycasted world
 * Handles depth-based occlusion using per-column depth buffer
 * Supports static weapon and enemy sprites
 */

#include "doom_sprite_renderer.h"
#include "doom_sprite_textures.h"
#include "doom_adapter.h"
#include "doom_feedback.h"
#include "doom_game_feel.h"
#include <Arduino.h>
#include <math.h>

namespace DoomSpriteRenderer {

// ============================================================================
// MODULE STATE
// ============================================================================

static bool g_initialized = false;
static const uint8_t MAX_SPRITES = 4;
static Sprite g_sprites[MAX_SPRITES];
static uint8_t g_sprite_count = 0;

// ============================================================================
// SPRITE TEXTURE DATA (Phase 3.2: Now using Doom-style textures from header)
// ============================================================================
// See doom_sprite_textures.h for all sprite definitions
// Textures are now in a dedicated module for better organization

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Get pixel value from texture at (tx, ty)
 * Phase 3.2: Uses new sprite texture format (16×16, 1-bit)
 * @param texture: Texture data pointer (PROGMEM)
 * @param tx: Texture X (0-15)
 * @param ty: Texture Y (0-15)
 * @return 1 if pixel is set, 0 if not
 */
static uint8_t getTexturePixel(const uint8_t* texture, uint8_t tx, uint8_t ty) {
    // Use the sprite texture sampling function from doom_sprite_textures
    return DoomSpriteTextures::sampleSprite(texture, tx, ty);
}

/**
 * Get texture data for sprite
 * Phase 3.2: Now uses state-based texture selection
 * @param texture_id: SPRITE_WEAPON or SPRITE_ENEMY
 * @param state: For enemies: ENEMY_ALIVE, ENEMY_HIT, ENEMY_DEAD
 *              For weapon: unused (weapon state checked at render time)
 * @param is_firing: For weapon only - whether firing state
 * @return Pointer to texture data in PROGMEM
 */
static const uint8_t* getTexture(uint8_t texture_id, uint8_t state = 0, bool is_firing = false) {
    if (texture_id == SPRITE_WEAPON) {
        // Weapon texture selection
        return DoomSpriteTextures::getWeaponSprite(is_firing);
    } else {
        // Enemy texture selection by combat state
        return DoomSpriteTextures::getEnemySprite(state);
    }
}

/**
 * Dither pixel using Bayer 2×2 ordered dither
 * Maps 1-bit sprite pixel to monochrome with dithering
 * @param pixel: 1 if sprite pixel on, 0 if off
 * @param x: Screen X coordinate
 * @param y: Screen Y coordinate
 * @return 1 if final pixel should be white, 0 if black
 */
static uint8_t ditherSpritePixel(uint8_t pixel, uint8_t x, uint8_t y) {
    if (!pixel) return 0;  // Off pixels stay off
    
    // Bayer 2×2 pattern ensures even dithering
    const uint8_t thresholds[2][2] = {
        { 64, 192 },
        { 0, 128 }
    };
    
    uint8_t threshold = thresholds[y & 1][x & 1];
    return (192 > threshold) ? 1 : 0;  // On pixels get dithered
}

/**
 * Draw sprite at framebuffer position with clipping
 * @param framebuffer: Target framebuffer
 * @param screen_x: Screen column where sprite starts (center-aligned)
 * @param screen_y: Screen row where sprite starts (center-aligned)
 * @param texture: Texture pointer (PROGMEM)
 * @param scale: Rendering scale (1.0 = full size)
 */
static void drawSpriteScaled(
    uint8_t* framebuffer,
    int16_t screen_x,
    int16_t screen_y,
    const uint8_t* texture,
    double scale
) {
    // Calculate scaled dimensions
    int16_t scaled_w = (int16_t)(SPRITE_WIDTH * scale);
    int16_t scaled_h = (int16_t)(SPRITE_HEIGHT * scale);
    
    // Center sprite on screen position
    int16_t start_x = screen_x - (scaled_w / 2);
    int16_t start_y = screen_y - (scaled_h / 2);
    
    // Render sprite with scaling and clipping
    for (int16_t sy = 0; sy < scaled_h; sy++) {
        int16_t screen_row = start_y + sy;
        
        // Vertical clipping
        if (screen_row < 0 || screen_row >= 64) continue;
        
        // Sample from texture with scaling
        uint8_t ty = (uint8_t)((sy * SPRITE_HEIGHT) / scaled_h);
        
        for (int16_t sx = 0; sx < scaled_w; sx++) {
            int16_t screen_col = start_x + sx;
            
            // Horizontal clipping
            if (screen_col < 0 || screen_col >= 128) continue;
            
            // Sample from texture
            uint8_t tx = (uint8_t)((sx * SPRITE_WIDTH) / scaled_w);
            uint8_t tex_pixel = getTexturePixel(texture, tx, ty);
            
            if (tex_pixel) {
                // Dither sprite pixel
                uint8_t final_pixel = ditherSpritePixel(tex_pixel, screen_col, screen_row);
                
                if (final_pixel) {
                    // Set pixel in framebuffer (column-major layout)
                    uint8_t byte_offset = screen_row / 8;
                    uint8_t bit_offset = screen_row & 0x07;
                    uint16_t fb_offset = (screen_col * 8) + byte_offset;
                    
                    framebuffer[fb_offset] |= (1 << bit_offset);
                }
            }
        }
    }
}

// ============================================================================
// PUBLIC API
// ============================================================================

void initialize() {
    if (g_initialized) return;
    
    memset(g_sprites, 0, sizeof(g_sprites));
    g_sprite_count = 0;
    
    Serial.println(F("[SPRITE] Module initialized"));
    Serial.println(F("[SPRITE] Doom-style sprite textures active (Phase 3.2)"));
    g_initialized = true;
}

bool addSprite(const Sprite& sprite) {
    if (g_sprite_count >= MAX_SPRITES) {
        Serial.println(F("[SPRITE] ERROR: Sprite list full"));
        return false;
    }
    
    g_sprites[g_sprite_count] = sprite;
    g_sprite_count++;
    
    Serial.printf("[SPRITE] Added sprite type %d at (%.1f, %.1f)\r\n",
                  sprite.type, sprite.x, sprite.y);
    
    return true;
}

void removeSprite(uint8_t index) {
    if (index >= g_sprite_count) return;
    
    // Shift remaining sprites down
    for (uint8_t i = index; i < g_sprite_count - 1; i++) {
        g_sprites[i] = g_sprites[i + 1];
    }
    
    g_sprite_count--;
}

void clearSprites() {
    g_sprite_count = 0;
}

// ============================================================================
// ENEMY SPAWNING (Phase 2.9)
// ============================================================================

static void spawnEnemiesInternal(
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    uint8_t enemy_count
) {
    if (!g_initialized) {
        Serial.println(F("[ENEMY] ERROR: Sprite module not initialized"));
        return;
    }
    
    // Clear existing enemies (but keep weapon)
    for (uint8_t i = 0; i < g_sprite_count; ) {
        if (g_sprites[i].type == SPRITE_ENEMY) {
            removeSprite(i);
        } else {
            i++;
        }
    }
    
    Serial.printf("[ENEMY] Spawning %d enemies\r\n", enemy_count);
    
    // Spawn enemies in formation
    for (uint8_t enemy_id = 0; enemy_id < enemy_count; enemy_id++) {
        // Calculate angle for this enemy (spread formation)
        double angle_offset = (double)(enemy_id - (INITIAL_ENEMY_COUNT - 1) / 2.0) * 
                             (ENEMY_SPAWN_ANGLE_SPREAD / (double)INITIAL_ENEMY_COUNT) * 
                             M_PI / 180.0;  // Convert degrees to radians
        
        // Rotate around player direction
        double spawn_dir_x = player_dir_x * cos(angle_offset) - player_dir_y * sin(angle_offset);
        double spawn_dir_y = player_dir_x * sin(angle_offset) + player_dir_y * cos(angle_offset);
        
        // Place enemy at spawn distance in that direction
        double enemy_x = player_x + spawn_dir_x * ENEMY_SPAWN_DISTANCE;
        double enemy_y = player_y + spawn_dir_y * ENEMY_SPAWN_DISTANCE;
        
        // Create enemy sprite
        Sprite enemy;
        enemy.x = enemy_x;
        enemy.y = enemy_y;
        enemy.type = SPRITE_ENEMY;
        enemy.texture_id = SPRITE_ENEMY;
        enemy.active = SPRITE_ACTIVE;
        enemy.enemy_state = ENEMY_ALIVE;
        enemy.hit_frame_counter = 0;
        enemy.enemy_id = enemy_id;
        enemy.ai_state = AI_IDLE;              // Phase 2.10: Initialize to IDLE
        enemy.attack_cooldown_ms = 0;          // Phase 2.10: Initialize cooldown timer
        
        if (addSprite(enemy)) {
            Serial.printf("[ENEMY] %d spawned at (%.1f, %.1f)\r\n", 
                         enemy_id, enemy_x, enemy_y);
        } else {
            Serial.printf("[ENEMY] %d spawn FAILED - list full\r\n", enemy_id);
            break;
        }
    }
    
    Serial.printf("[ENEMY] Spawn complete - %d enemies active\r\n", g_sprite_count - 1);
}

void spawnEnemies(
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y
) {
    spawnEnemiesInternal(
        player_x,
        player_y,
        player_dir_x,
        player_dir_y,
        INITIAL_ENEMY_COUNT
    );
}

void spawnEnemies(
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    uint8_t requested_count
) {
    uint8_t count = (requested_count == 0) ? INITIAL_ENEMY_COUNT : requested_count;
    spawnEnemiesInternal(
        player_x,
        player_y,
        player_dir_x,
        player_dir_y,
        count
    );
}

void renderWeapon(uint8_t* framebuffer) {
    // Find weapon sprite in list
    for (uint8_t i = 0; i < g_sprite_count; i++) {
        if (g_sprites[i].type == SPRITE_WEAPON && g_sprites[i].active == SPRITE_ACTIVE) {
            // Render at fixed screen position (bottom-right area)
            // Screen position: right side, near bottom
            // 128×64 display → weapon at (96, 52)
            int16_t screen_x = 96;
            int16_t screen_y = 48;
            
            // Check if weapon flash effect should be applied (Phase 2.7)
            bool flash = DoomAdapter::getWeaponFlash();
            bool is_firing = flash;  // Flash = firing state (Phase 3.2)
            
            // Get texture based on firing state (Phase 3.2)
            const uint8_t* texture = getTexture(SPRITE_WEAPON, 0, is_firing);
            
            if (flash) {
                // Fill weapon area with white (all pixels set)
                // Weapon is 16×16 pixels scaled by 1.5 = ~24×24 pixels
                for (int16_t y = screen_y - 12; y < screen_y + 12 && y >= 0 && y < 64; y++) {
                    for (int16_t x = screen_x - 12; x < screen_x + 12 && x >= 0 && x < 128; x++) {
                        uint16_t byte_idx = (y / 8) * 128 + x;
                        uint8_t bit_idx = y % 8;
                        framebuffer[byte_idx] |= (1 << bit_idx);  // Set bit to white
                    }
                }
                Serial.println(F("[SPRITE] Weapon FIRE rendered (Phase 3.2)"));
            } else {
                // Normal weapon rendering with idle texture
                drawSpriteScaled(framebuffer, screen_x, screen_y, texture, 1.5);
                Serial.println(F("[SPRITE] Weapon IDLE rendered (Phase 3.2)"));
            }
            
            return;
        }
    }
}

bool isSpriteVisible(double sprite_dist, uint8_t screen_x, uint8_t depth) {
    if (screen_x >= 128) return false;
    
    // Convert depth buffer value to distance (rough mapping)
    double wall_dist = depth * 0.2;  // Scale factor
    
    // Sprite visible if closer than wall
    return sprite_dist < wall_dist;
}

void renderSprites(
    uint8_t* framebuffer,
    const uint8_t* depth_buffer,
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    double player_plane_x,
    double player_plane_y
) {
    if (!g_initialized || g_sprite_count == 0) {
        return;
    }
    
    // Render weapon first (always on top)
    renderWeapon(framebuffer);
    
    // ========================================================================
    // Phase 2.9: DEPTH-SORTING FOR CORRECT OCCLUSION
    // ========================================================================
    // Collect all visible enemies with their distances
    struct SpriteRenderJob {
        uint8_t index;
        double distance;
    };
    
    SpriteRenderJob jobs[MAX_SPRITES - 1];  // Max 3 enemies (weapon is slot 0)
    uint8_t job_count = 0;
    
    // Phase 2.8/2.9: Handle enemy hit state transitions FIRST
    // This must happen before distance calculation
    for (uint8_t i = 0; i < g_sprite_count; i++) {
        Sprite& sprite = g_sprites[i];
        
        // Only process enemies in HIT state
        if (sprite.type == SPRITE_ENEMY && sprite.enemy_state == ENEMY_HIT) {
            if (sprite.hit_frame_counter > 0) {
                sprite.hit_frame_counter--;
            } else {
                // Transition from HIT to DEAD
                sprite.enemy_state = ENEMY_DEAD;
                sprite.active = false;
                Serial.printf("[ENEMY] %d DEAD (HIT resolved)\r\n", sprite.enemy_id);
                
                // Phase 3.4: Trigger enemy death micro-feedback
                DoomGameFeel::triggerEnemyDeath();
                
                // Notify game flow about enemy death (Phase 2.15)
                DoomGameFlow::notifyEnemyKilled();
            }
        }
    }
    
    // Collect visible sprites with distances
    for (uint8_t i = 0; i < g_sprite_count; i++) {
        Sprite& sprite = g_sprites[i];
        
        if (sprite.active != SPRITE_ACTIVE) continue;
        if (sprite.type == SPRITE_WEAPON) continue;  // Weapon rendered separately
        if (sprite.type != SPRITE_ENEMY) continue;
        if (sprite.enemy_state == ENEMY_DEAD) continue;  // Skip dead enemies
        
        // Calculate vector from player to sprite
        double sprite_vec_x = sprite.x - player_x;
        double sprite_vec_y = sprite.y - player_y;
        
        // Calculate distance
        double sprite_dist = sqrt(sprite_vec_x * sprite_vec_x + 
                                 sprite_vec_y * sprite_vec_y);
        
        // Cull sprites too far away
        if (sprite_dist > 50.0) continue;
        
        // Transform to camera space (relative to player direction)
        double transform_x = sprite_vec_x * player_dir_x + 
                            sprite_vec_y * (-player_dir_y);
        double transform_y = sprite_vec_x * player_plane_x + 
                            sprite_vec_y * player_plane_y;
        
        // Behind camera - skip
        if (transform_x <= 0.1) continue;
        
        // Project to screen space
        double screen_x = 64.0 + (transform_y / transform_x) * 64.0;
        
        // Off-screen horizontally - skip
        if (screen_x < 0 || screen_x >= 128) continue;
        
        // Add to render job list
        if (job_count < (MAX_SPRITES - 1)) {
            jobs[job_count].index = i;
            jobs[job_count].distance = sprite_dist;
            job_count++;
        }
    }
    
    // ========================================================================
    // SIMPLE BUBBLE SORT BY DISTANCE (back-to-front)
    // ========================================================================
    // Sort jobs by distance (farthest first)
    for (uint8_t i = 0; i < job_count; i++) {
        for (uint8_t j = i + 1; j < job_count; j++) {
            if (jobs[i].distance < jobs[j].distance) {
                // Swap
                SpriteRenderJob temp = jobs[i];
                jobs[i] = jobs[j];
                jobs[j] = temp;
            }
        }
    }
    
    // ========================================================================
    // RENDER SPRITES IN SORTED ORDER (back-to-front)
    // ========================================================================
    for (uint8_t job_idx = 0; job_idx < job_count; job_idx++) {
        uint8_t i = jobs[job_idx].index;
        Sprite& sprite = g_sprites[i];
        double sprite_dist = jobs[job_idx].distance;
        
        // Recalculate projection (already done above, but needed for rendering)
        double sprite_vec_x = sprite.x - player_x;
        double sprite_vec_y = sprite.y - player_y;
        
        double transform_x = sprite_vec_x * player_dir_x + 
                            sprite_vec_y * (-player_dir_y);
        double transform_y = sprite_vec_x * player_plane_x + 
                            sprite_vec_y * player_plane_y;
        
        double screen_x = 64.0 + (transform_y / transform_x) * 64.0;
        uint8_t screen_col = (uint8_t)screen_x;
        
        // Check depth visibility
        if (!isSpriteVisible(sprite_dist, screen_col, depth_buffer[screen_col])) {
            continue;
        }
        
        // Calculate sprite screen Y position
        // Distance-based scaling: closer = larger
        double scale = 32.0 / sprite_dist;
        scale = (scale > 2.0) ? 2.0 : scale;  // Cap at 2x
        int16_t screen_y = 32;  // Vertical center
        
        // Phase 3.2: Render with state-based texture
        bool is_hit = (sprite.enemy_state == ENEMY_HIT);
        if (is_hit) {
            // Visual feedback: White flash + vertical offset (Phase 2.8)
            screen_y += ENEMY_HIT_OFFSET_Y;
            
            // Render HIT state texture with flash overlay
            const uint8_t* texture = getTexture(sprite.texture_id, sprite.enemy_state);
            drawSpriteScaled(framebuffer, screen_x, screen_y, texture, scale);
            
            // Flash effect: Fill sprite area with white
            int16_t sprite_size = (int16_t)(16 * scale / 2.0);
            
            for (int16_t y = screen_y - sprite_size; y < screen_y + sprite_size && y >= 0 && y < 64; y++) {
                for (int16_t x = (int16_t)screen_x - sprite_size; x < (int16_t)screen_x + sprite_size && x >= 0 && x < 128; x++) {
                    uint16_t byte_idx = (y / 8) * 128 + x;
                    uint8_t bit_idx = y % 8;
                    framebuffer[byte_idx] |= (1 << bit_idx);  // Set to white
                }
            }
            
            Serial.printf("[SPRITE] Enemy %d HIT state rendered (Phase 3.2)\r\n", sprite.enemy_id);
        } else {
            // Normal sprite rendering with appropriate state texture (Phase 3.2)
            const uint8_t* texture = getTexture(sprite.texture_id, sprite.enemy_state);
            drawSpriteScaled(framebuffer, screen_x, screen_y, texture, scale);
            
            Serial.printf("[SPRITE] Enemy %d IDLE rendered at col=%d, dist=%.1f (Phase 3.2)\r\n",
                         sprite.enemy_id, screen_col, sprite_dist);
        }
    }
}

const Sprite* getSprite(uint8_t index) {
    if (index >= g_sprite_count) return nullptr;
    return &g_sprites[index];
}

uint8_t getSpriteCount() {
    return g_sprite_count;
}

bool isInitialized() {
    return g_initialized;
}

// ============================================================================
// COMBAT DETECTION
// ============================================================================

// Track fire button state to avoid continuous fire
static bool g_fire_was_pressed = false;

uint8_t checkHitDetection(
    const uint8_t* depth_buffer,
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    double player_plane_x,
    double player_plane_y,
    double& hit_distance
) {
    if (!g_initialized || g_sprite_count == 0) return 255;
    
    const uint8_t CENTRAL_RAY = 64;
    const uint8_t MIN_COL = (CENTRAL_RAY > COMBAT_HIT_ANGLE_THRESHOLD) ? 
        (CENTRAL_RAY - COMBAT_HIT_ANGLE_THRESHOLD) : 0;
    const uint8_t MAX_COL = (CENTRAL_RAY + COMBAT_HIT_ANGLE_THRESHOLD);
    
    uint8_t hit_sprite_idx = 255;
    double closest_hit_dist = 1e9;
    
    // Check each active enemy sprite
    for (uint8_t i = 0; i < g_sprite_count; i++) {
        const Sprite& sprite = g_sprites[i];
        
        // Skip non-enemies or dead/inactive sprites
        if (sprite.type != SPRITE_ENEMY) continue;
        if (!sprite.active) continue;
        if (sprite.enemy_state != ENEMY_ALIVE) continue;
        
        // Calculate distance from player to sprite
        double dx = sprite.x - player_x;
        double dy = sprite.y - player_y;
        double sprite_dist = sqrt(dx * dx + dy * dy);
        
        // Skip sprites beyond hit range
        if (sprite_dist > COMBAT_HIT_DISTANCE_THRESHOLD) continue;
        
        // Project sprite to screen
        // Use perpendicular distance to player's forward direction
        double perp_dist = dx * player_dir_x + dy * player_dir_y;
        if (perp_dist <= 0) continue;  // Behind player
        
        // Calculate angle offset from center
        double angle_offset = (dx * player_dir_y - dy * player_dir_x) / perp_dist;
        double sprite_screen_x = 64.0 + (angle_offset * 128.0 / 2.0);  // ~90 degree FOV
        
        // Check if sprite projects to central hit zone
        if (sprite_screen_x < MIN_COL || sprite_screen_x > MAX_COL) continue;
        
        uint8_t col_idx = (uint8_t)sprite_screen_x;
        if (col_idx >= 128) col_idx = 127;
        
        // Convert depth buffer value (scaled 0-255) to approximate world distance
        // depth_buffer is normalized (0=far, 255=close)
        // We use it as a rough comparison
        uint8_t wall_depth = depth_buffer[col_idx];
        
        // Sprite hits if closer than wall at this column
        // Use a simple threshold: sprite dist < (wall_depth normalized)
        double wall_approx_dist = (255 - wall_depth) * 0.5 + 1.0;  // Scale to roughly 1-128 units
        
        if (sprite_dist < wall_approx_dist && sprite_dist < closest_hit_dist) {
            closest_hit_dist = sprite_dist;
            hit_sprite_idx = i;
        }
    }
    
    if (hit_sprite_idx != 255) {
        hit_distance = closest_hit_dist;
    }
    
    return hit_sprite_idx;
}

// ============================================================================
// ENEMY AI STATE MACHINE (Phase 2.10)
// ============================================================================

void updateEnemyAI(
    double player_x,
    double player_y,
    uint32_t current_time_ms
) {
    if (!g_initialized || g_sprite_count == 0) return;
    
    for (uint8_t i = 0; i < g_sprite_count; i++) {
        Sprite& enemy = g_sprites[i];
        
        // Skip non-enemies or inactive sprites
        if (enemy.type != SPRITE_ENEMY) continue;
        if (enemy.active != SPRITE_ACTIVE) continue;
        if (enemy.enemy_state == ENEMY_DEAD) continue;
        
        // Calculate distance to player
        double dx = player_x - enemy.x;
        double dy = player_y - enemy.y;
        double dist_sq = dx*dx + dy*dy;
        double dist = sqrt(dist_sq);
        
        // AI state machine
        switch (enemy.ai_state) {
            case AI_IDLE:
                // Check if player is close enough to detect
                if (dist < ENEMY_AWARENESS_DISTANCE) {
                    enemy.ai_state = AI_CHASE;
                    Serial.printf("[AI] Enemy %d IDLE→CHASE (dist=%.1f)\r\n", 
                                 enemy.enemy_id, dist);
                }
                break;
                
            case AI_CHASE:
                // Check if close enough to attack
                if (dist < ENEMY_ATTACK_DISTANCE) {
                    enemy.ai_state = AI_ATTACK;
                    enemy.attack_cooldown_ms = 0;  // Reset cooldown for first attack
                    Serial.printf("[AI] Enemy %d CHASE→ATTACK\r\n", enemy.enemy_id);
                } else if (dist < ENEMY_AWARENESS_DISTANCE) {
                    // Move toward player
                    if (dist > 0.1) {  // Avoid division by zero
                        double move_dx = (dx / dist) * ENEMY_MOVE_SPEED;
                        double move_dy = (dy / dist) * ENEMY_MOVE_SPEED;
                        
                        enemy.x += move_dx;
                        enemy.y += move_dy;
                    }
                } else {
                    // Player out of range, return to idle
                    enemy.ai_state = AI_IDLE;
                    Serial.printf("[AI] Enemy %d CHASE→IDLE (out of range)\r\n", 
                                 enemy.enemy_id);
                }
                break;
                
            case AI_ATTACK:
                // Check if still in attack range
                if (dist > ENEMY_ATTACK_DISTANCE) {
                    enemy.ai_state = AI_CHASE;
                    Serial.printf("[AI] Enemy %d ATTACK→CHASE (out of range)\r\n", 
                                 enemy.enemy_id);
                } else {
                    // Perform attack with cooldown
                    if (enemy.attack_cooldown_ms == 0 || 
                        (current_time_ms - enemy.attack_cooldown_ms) >= ENEMY_ATTACK_COOLDOWN_MS) {
                        // Attack the player
                        DoomAdapter::registerDamageSource(enemy.x, enemy.y);
                        DoomAdapter::applyPlayerDamage(ENEMY_ATTACK_DAMAGE);
                        enemy.attack_cooldown_ms = current_time_ms;
                        Serial.printf("[AI] Enemy %d ATTACK→player damage applied\r\n", 
                                     enemy.enemy_id);
                    }
                }
                break;
                
            case AI_HIT:
                // Hit state is handled in renderSprites()
                // Just check if we should go back to chase
                if (enemy.enemy_state != ENEMY_HIT) {
                    enemy.ai_state = AI_CHASE;
                    Serial.printf("[AI] Enemy %d HIT→CHASE (recovered)\r\n", 
                                 enemy.enemy_id);
                }
                break;
                
            case AI_DEAD:
                // Dead enemies do nothing
                break;
        }
    }
}

void handleCombat(
    bool fire_pressed,
    const uint8_t* depth_buffer,
    double player_x,
    double player_y,
    double player_dir_x,
    double player_dir_y,
    double player_plane_x,
    double player_plane_y
) {
    if (!g_initialized || !depth_buffer) return;
    
    // Check for fire button press (edge trigger, not continuous)
    if (fire_pressed && !g_fire_was_pressed) {
        // Fire pressed this frame
        Serial.printf("[COMBAT] FIRE\r\n");
        
        double hit_distance = 0.0;
        uint8_t hit_idx = checkHitDetection(
            depth_buffer,
            player_x, player_y,
            player_dir_x, player_dir_y,
            player_plane_x, player_plane_y,
            hit_distance
        );
        
        if (hit_idx != 255 && hit_idx < g_sprite_count) {
            // Hit confirmed
            Sprite& hit_sprite = g_sprites[hit_idx];
            
            Serial.printf("[COMBAT] HIT enemy #%d at dist=%.1f\r\n", 
                         hit_sprite.enemy_id, hit_distance);
            
            // Mark enemy as HIT (not dead yet - Phase 2.8 visual feedback)
            hit_sprite.enemy_state = ENEMY_HIT;
            hit_sprite.hit_frame_counter = ENEMY_HIT_DURATION_FRAMES;
            
            Serial.printf("[ENEMY] %d ENTER HIT\r\n", hit_sprite.enemy_id);

            // Phase 2.14: Hit confirm feedback at crosshair
            DoomFeedback::triggerHitConfirm();

            // Phase 3.4: Trigger enemy hit micro-feedback
            DoomGameFeel::triggerEnemyHit();
        }
    }
    
    // Update fire button state for next frame
    g_fire_was_pressed = fire_pressed;
}

}  // namespace DoomSpriteRenderer
