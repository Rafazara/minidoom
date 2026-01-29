#include "doom_world_renderer.h"
#include "doom_wall_textures.h"
#include <Arduino.h>
#include <math.h>

namespace DoomWorldRenderer {

// ============================================================================
// CONFIGURATION FOR OLED 128x64
// ============================================================================
static const uint8_t RENDER_WIDTH = 128;    // OLED full width
static const uint8_t RENDER_HEIGHT = 64;    // OLED full height
static const uint8_t RAY_STEP = 1;          // Cast ray for every column (max quality)
static const uint8_t MAX_RENDER_DEPTH = 12; // Maximum ray tracing depth
static const uint8_t MAX_DISTANCE = 255;    // Max distance (uint8_t limit)

// Gradient levels for dithering (simulating shading from Doom Nano)
static const uint8_t GRADIENT_LEVELS = 5;   // 5 brightness levels

// ============================================================================
// FRAMEBUFFER & STATE
// ============================================================================
static uint8_t g_worldFramebuffer[1024];    // 128×64 = 1024 bytes
static bool g_initialized = false;
static uint8_t g_depthBuffer[128];          // Z-buffer for depth (one per column)

/**
 * Get gradient value at position based on depth
 * Simulates Doom Nano's gradient-based shading
 * 
 * @param distance: Normalized distance 0-255
 * @return: 0-4 representing shade level (0=closest/brightest, 4=farthest/darkest)
 */
static inline uint8_t getShadeLevel(uint8_t distance) {
    // Map distance 0-255 to shade levels 0-4
    // Closer = brighter, farther = darker
    uint8_t shade = (distance * GRADIENT_LEVELS) / MAX_DISTANCE;
    if (shade > GRADIENT_LEVELS - 1) shade = GRADIENT_LEVELS - 1;
    if (shade < 0) shade = 0;
    return shade;
}

/**
 * Simple threshold-based dithering for a shade level
 * Converts gradient level to monochrome bit using ordered dithering
 * 
 * @param shade: Shade level 0-4 (0=bright, 4=dark)
 * @param x: X coordinate (for dither pattern)
 * @param y: Y coordinate (for dither pattern)
 * @return: 1 if pixel should be ON, 0 if OFF
 */
static inline uint8_t ditherShade(uint8_t shade, uint8_t x, uint8_t y) {
    // Bayer 2×2 pattern for dithering
    // Higher shade = more likely to be ON (darker)
    const uint8_t thresholds[2][2] = {
        { 64, 192 },
        { 0, 128 }
    };
    
    // Map shade to effective "brightness" for dithering
    // shade=0 (bright) → low output (less likely ON)
    // shade=4 (dark) → high output (more likely ON)
    uint8_t dither_val = shade * 51;  // 0, 51, 102, 153, 204, 255
    uint8_t threshold = thresholds[y & 1][x & 1];
    
    return (dither_val > threshold) ? 1 : 0;
}

/**
 * Draw vertical line using texture sampling and dithering
 * 
 * @param x: Column position (0-127)
 * @param y_start: Start Y (0-63)
 * @param y_end: End Y (0-63)
 * @param shade: Shade level 0-4
 * @param texture: Pointer to texture data in PROGMEM (or nullptr for solid)
 * @param texture_x: X coordinate in texture (0-15)
 * @param wall_height: Height of wall in screen pixels (for texture mapping)
 */
static void drawDitheredVLine(
    uint8_t x, int16_t y_start, int16_t y_end, uint8_t shade,
    const uint8_t* texture = nullptr, uint8_t texture_x = 0, double wall_height = 0.0
) {
    // Clamp to screen bounds
    int16_t y_min = (y_start < y_end) ? y_start : y_end;
    int16_t y_max = (y_start > y_end) ? y_start : y_end;
    
    if (y_min < 0) y_min = 0;
    if (y_max >= RENDER_HEIGHT) y_max = RENDER_HEIGHT - 1;
    if (x >= RENDER_WIDTH) x = RENDER_WIDTH - 1;
    
    // If no texture provided, use solid dithering (legacy)
    if (texture == nullptr) {
        for (int16_t y = y_min; y <= y_max; y++) {
            uint8_t bit = 7 - (y & 7);
            uint16_t byte_idx = (y / 8) * RENDER_WIDTH + x;
            
            if (byte_idx >= 1024) continue;  // Out of bounds
            
            uint8_t dither_bit = ditherShade(shade, x, y);
            
            if (dither_bit) {
                g_worldFramebuffer[byte_idx] |= (1 << bit);
            } else {
                g_worldFramebuffer[byte_idx] &= ~(1 << bit);
            }
        }
        return;
    }
    
    // Texture-mapped rendering
    // Map screen Y to texture Y based on wall height
    double texture_height = DoomWallTextures::TEXTURE_SIZE;
    
    for (int16_t y = y_min; y <= y_max; y++) {
        // Calculate texture Y coordinate
        // Map screen column to texture column based on wall height and position
        double frac_y = (double)(y - y_min) / (y_max - y_min + 1.0);
        uint8_t texture_y = (uint8_t)(frac_y * texture_height);
        texture_y &= (DoomWallTextures::TEXTURE_SIZE - 1);
        
        // Sample texture
        uint8_t texture_bit = DoomWallTextures::sampleTexture(texture, texture_x, texture_y);
        
        // Apply dithering to texture pattern
        uint8_t bit = y & 7;
        uint16_t byte_idx = (y / 8) * RENDER_WIDTH + x;
        
        if (byte_idx >= 1024) continue;  // Out of bounds
        
        // Combine texture pattern with dithered shading
        // If texture pixel is ON, use dithered shade; if OFF, keep it OFF
        uint8_t dither_bit = ditherShade(shade, x, y);
        uint8_t final_bit = texture_bit & dither_bit;
        
        if (final_bit) {
            g_worldFramebuffer[byte_idx] |= (1 << bit);
        } else {
            g_worldFramebuffer[byte_idx] &= ~(1 << bit);
        }
    }
}

/**
 * Get block type at map coordinates
 * Doom Nano level format: packed nibbles (4 bits per cell)
 * 
 * @param level: Level data array
 * @param x: Map X coordinate (0-63)
 * @param y: Map Y coordinate (0-56)
 * @return: Block type (0=empty, 15=wall, etc.)
 */
static inline uint8_t getBlockAt(const uint8_t* level, uint8_t x, uint8_t y) {
    // Level data format from Doom Nano: nibbles packed
    // LEVEL_WIDTH = 64, LEVEL_HEIGHT = 57
    // Each cell = 4 bits (one nibble)
    // Row is 64/2 = 32 bytes
    
    if (x >= 64 || y >= 57 || level == nullptr) {
        return 15;  // Return wall (safe default)
    }
    
    uint16_t row_start = y * 32;  // 64 cells / 2 per byte = 32 bytes per row
    uint8_t byte_pos = row_start + (x / 2);
    uint8_t nibble_pos = (x & 1);
    
    uint8_t byte_val = level[byte_pos];
    
    if (nibble_pos == 0) {
        return (byte_val >> 4) & 0x0F;  // High nibble
    } else {
        return byte_val & 0x0F;         // Low nibble
    }
}

/**
 * Simplified raycasting algorithm
 * Adapted from Doom Nano, optimized for OLED resolution
 * 
 * Casts rays from player position through camera plane
 * Performs DDA (Digital Differential Analyzer) wall detection
 * Draws dithered vertical lines based on hit distance
 */
static void raycaster(
    const uint8_t* level,
    double player_x, double player_y,
    double dir_x, double dir_y,
    double plane_x, double plane_y,
    double view_height
) {
    // Cast one ray per screen column
    for (uint8_t x = 0; x < RENDER_WIDTH; x += RAY_STEP) {
        // Calculate ray direction through camera plane
        double camera_x = 2.0 * x / RENDER_WIDTH - 1.0;
        double ray_x = dir_x + plane_x * camera_x;
        double ray_y = dir_y + plane_y * camera_x;
        
        // Start position in map
        int16_t map_x = (int16_t)player_x;
        int16_t map_y = (int16_t)player_y;
        
        // DDA algorithm setup
        double delta_x = fabs(1.0 / ray_x);
        double delta_y = fabs(1.0 / ray_y);
        
        int8_t step_x = (ray_x < 0) ? -1 : 1;
        int8_t step_y = (ray_y < 0) ? -1 : 1;
        
        double side_x, side_y;
        
        if (ray_x < 0) {
            side_x = (player_x - map_x) * delta_x;
        } else {
            side_x = (map_x + 1.0 - player_x) * delta_x;
        }
        
        if (ray_y < 0) {
            side_y = (player_y - map_y) * delta_y;
        } else {
            side_y = (map_y + 1.0 - player_y) * delta_y;
        }
        
        // March ray until wall hit
        uint8_t depth = 0;
        bool hit = false;
        uint8_t side = 0;  // 0 = vertical wall, 1 = horizontal wall
        
        while (!hit && depth < MAX_RENDER_DEPTH) {
            // DDA step
            if (side_x < side_y) {
                side_x += delta_x;
                map_x += step_x;
                side = 0;
            } else {
                side_y += delta_y;
                map_y += step_y;
                side = 1;
            }
            
            // Check block type
            uint8_t block = getBlockAt(level, (uint8_t)map_x, (uint8_t)map_y);
            
            if (block == 0x0F) {  // Wall (0xF in Doom Nano)
                hit = true;
            }
            
            depth++;
        }
        
        // Calculate distance if hit
        if (hit) {
            double distance;
            double wall_hit_x = 0.0;  // Exact point where ray hit the wall
            
            if (side == 0) {
                distance = (map_x - player_x + (1 - step_x) / 2.0) / ray_x;
                // Calculate Y coordinate of hit point
                wall_hit_x = player_y + distance * ray_y;
            } else {
                distance = (map_y - player_y + (1 - step_y) / 2.0) / ray_y;
                // Calculate X coordinate of hit point
                wall_hit_x = player_x + distance * ray_x;
            }
            
            // Clamp distance to valid range
            if (distance < 0.1) distance = 0.1;
            if (distance > 50.0) distance = 50.0;
            
            // Store in depth buffer (for sprite rendering later)
            uint8_t depth_val = (uint8_t)(((uint32_t)(distance * 5.0)) & 0xFF);  // Scale to byte
            g_depthBuffer[x / RAY_STEP] = depth_val;
            
            // Calculate wall height
            double wall_height = RENDER_HEIGHT / distance;
            
            // Calculate wall Y coordinates
            double center_y = RENDER_HEIGHT / 2.0 + view_height / distance;
            int16_t y_top = (int16_t)(center_y - wall_height / 2.0);
            int16_t y_bottom = (int16_t)(center_y + wall_height / 2.0);
            
            // Map distance to shade level
            uint8_t shade = getShadeLevel((uint8_t)((distance / 50.0) * MAX_DISTANCE));
            
            // Add wall normal shading (side walls slightly darker)
            if (side == 1) {
                shade += 1;  // Horizontal walls slightly darker
                if (shade > GRADIENT_LEVELS - 1) shade = GRADIENT_LEVELS - 1;
            }
            
            // Calculate texture X coordinate from hit point
            // Extract fractional part and map to texture range
            uint8_t texture_x = (uint8_t)(fabs(wall_hit_x) * DoomWallTextures::TEXTURE_SIZE);
            texture_x &= (DoomWallTextures::TEXTURE_SIZE - 1);
            
            // Select texture (for now, use brick as default, can extend later)
            const uint8_t* texture = DoomWallTextures::WALL_BRICK;
            
            // Draw textured vertical line
            drawDitheredVLine(x, y_top, y_bottom, shade, texture, texture_x, wall_height);
        } else {
            // No hit - draw ceiling (very dark or black)
            g_depthBuffer[x / RAY_STEP] = MAX_DISTANCE;
        }
    }
}

/**
 * Initialize world renderer
 */
void initialize() {
    if (g_initialized) {
        return;
    }
    
    Serial.println(F("[DOOM-WORLD] Initializing world renderer..."));
    Serial.println(F("[DOOM-WORLD] Wall texture sampling active (Phase 3.1)"));
    
    // Clear framebuffer
    memset(g_worldFramebuffer, 0, sizeof(g_worldFramebuffer));
    memset(g_depthBuffer, 0xFF, sizeof(g_depthBuffer));
    
    Serial.println(F("[DOOM-WORLD] World renderer ready (128x64 raycasting + wall textures)"));
    g_initialized = true;
}

/**
 * Render 3D world view
 */
void renderWorld(
    const uint8_t* level,
    double player_x, double player_y,
    double dir_x, double dir_y,
    double plane_x, double plane_y,
    double view_height
) {
    if (!g_initialized || level == nullptr) {
        return;
    }
    
    // Clear framebuffer
    memset(g_worldFramebuffer, 0, sizeof(g_worldFramebuffer));
    
    // Perform raycasting
    raycaster(level, player_x, player_y, dir_x, dir_y, plane_x, plane_y, view_height);
    
    Serial.println(F("[DOOM-WORLD] World frame rendered"));
}

/**
 * Get rendered world framebuffer
 */
uint8_t* getWorldFramebuffer() {
    return g_worldFramebuffer;
}

/**
 * Get depth buffer for sprite occlusion testing
 */
const uint8_t* getDepthBuffer() {
    return g_depthBuffer;
}

/**
 * Check initialization status
 */
bool isInitialized() {
    return g_initialized;
}

}  // namespace DoomWorldRenderer
