#ifndef DOOM_WALL_TEXTURES_H
#define DOOM_WALL_TEXTURES_H

#include <stdint.h>
#include <pgmspace.h>

/**
 * Wall Textures - Phase 3.1
 * 
 * 16×16 pixel monochrome textures stored in PROGMEM
 * Each texture = 32 bytes (16 pixels wide × 16 pixels tall / 8 bits per byte)
 * Row-major format: each row = 2 bytes (16 pixels packed as bits)
 * 
 * Bit 0 = pixel ON (white), Bit 1 = pixel OFF (black)
 */

namespace DoomWallTextures {

// ============================================================================
// BRICK WALL - Classic Doom brick pattern
// 16×16 monochrome brick-like texture with mortar lines
// ============================================================================
static const uint8_t WALL_BRICK[32] PROGMEM = {
    0b11111111, 0b11111111,  // Row 0: ████████████████
    0b10000001, 0b00000001,  // Row 1: █      ███
    0b10000001, 0b00000001,  // Row 2: █      ███
    0b10000001, 0b00000001,  // Row 3: █      ███
    0b11111111, 0b11111111,  // Row 4: ████████████████
    0b00011000, 0b11000001,  // Row 5:    ██  ██      █
    0b00011000, 0b11000001,  // Row 6:    ██  ██      █
    0b00011000, 0b11000001,  // Row 7:    ██  ██      █
    0b11111111, 0b11111111,  // Row 8: ████████████████
    0b10000001, 0b00000001,  // Row 9: █      ███
    0b10000001, 0b00000001,  // Row 10: █      ███
    0b10000001, 0b00000001,  // Row 11: █      ███
    0b11111111, 0b11111111,  // Row 12: ████████████████
    0b00011000, 0b11000001,  // Row 13:    ██  ██      █
    0b00011000, 0b11000001,  // Row 14:    ██  ██      █
    0b00011000, 0b11000001,  // Row 15:    ██  ██      █
};

// ============================================================================
// METAL WALL - Industrial/tech metal grating pattern
// 16×16 monochrome metal grating with horizontal and vertical lines
// ============================================================================
static const uint8_t WALL_METAL[32] PROGMEM = {
    0b11111111, 0b11111111,  // Row 0: ████████████████
    0b10001000, 0b10001000,  // Row 1: █   █   █   █   
    0b10001000, 0b10001000,  // Row 2: █   █   █   █   
    0b11111111, 0b11111111,  // Row 3: ████████████████
    0b10001000, 0b10001000,  // Row 4: █   █   █   █   
    0b10001000, 0b10001000,  // Row 5: █   █   █   █   
    0b11111111, 0b11111111,  // Row 6: ████████████████
    0b10001000, 0b10001000,  // Row 7: █   █   █   █   
    0b10001000, 0b10001000,  // Row 8: █   █   █   █   
    0b11111111, 0b11111111,  // Row 9: ████████████████
    0b10001000, 0b10001000,  // Row 10: █   █   █   █   
    0b10001000, 0b10001000,  // Row 11: █   █   █   █   
    0b11111111, 0b11111111,  // Row 12: ████████████████
    0b10001000, 0b10001000,  // Row 13: █   █   █   █   
    0b10001000, 0b10001000,  // Row 14: █   █   █   █   
    0b11111111, 0b11111111,  // Row 15: ████████████████
};

// ============================================================================
// STONE WALL - Rough stone/rock pattern
// 16×16 monochrome irregular stone/rock texture
// ============================================================================
static const uint8_t WALL_STONE[32] PROGMEM = {
    0b11011101, 0b10110111,  // Row 0: ██ ███ █ ██ ███
    0b10111011, 0b11101110,  // Row 1: █ ███ ████ ███ 
    0b11101110, 0b11011011,  // Row 2: ███ ███ ██ ██
    0b11011011, 0b10111010,  // Row 3: ██ ██ █ ███ █ 
    0b10110111, 0b11010111,  // Row 4: █ ██ ███ █ ███
    0b11101110, 0b10110110,  // Row 5: ███ ███ █ ██ ██
    0b11011011, 0b11101110,  // Row 6: ██ ██ ████ ███ 
    0b10111010, 0b11011101,  // Row 7: █ ███ █ ██ ███ █
    0b11011101, 0b10110111,  // Row 8: ██ ███ █ ██ ███
    0b10111011, 0b11101110,  // Row 9: █ ███ ████ ███ 
    0b11101110, 0b11011011,  // Row 10: ███ ███ ██ ██
    0b11011011, 0b10111010,  // Row 11: ██ ██ █ ███ █ 
    0b10110111, 0b11010111,  // Row 12: █ ██ ███ █ ███
    0b11101110, 0b10110110,  // Row 13: ███ ███ █ ██ ██
    0b11011011, 0b11101110,  // Row 14: ██ ██ ████ ███ 
    0b10111010, 0b11011101,  // Row 15: █ ███ █ ██ ███ █
};

// ============================================================================
// Texture size constant
// ============================================================================
constexpr uint8_t TEXTURE_SIZE = 16;
constexpr uint8_t TEXTURE_BYTES = 32;  // 16×16 pixels / 8 bits per byte = 32 bytes

// ============================================================================
// Texture selection by wall block type (optional)
// Map block type to texture ID for future use
// ============================================================================
static const uint8_t getTextureIdForBlock(uint8_t block_type) {
    // Block type 0xF = wall (from Doom Nano)
    // You could extend this for different wall types later
    // For now, use cyclic pattern based on coordinates for visual variety
    switch (block_type) {
        case 0x0F:
            return 0;  // Default brick
        default:
            return 0;  // Default brick
    }
}

// ============================================================================
// Texture array for indexed access (future proof)
// ============================================================================
static const uint8_t* const TEXTURE_ARRAY[3] PROGMEM = {
    WALL_BRICK,
    WALL_METAL,
    WALL_STONE
};

constexpr uint8_t TEXTURE_COUNT = 3;

/**
 * Get texture data by index
 * Returns pointer to texture in PROGMEM
 * 
 * @param texture_id: 0=brick, 1=metal, 2=stone
 * @return: Pointer to 32-byte texture data in PROGMEM
 */
static inline const uint8_t* getTexture(uint8_t texture_id) {
    if (texture_id >= TEXTURE_COUNT) {
        texture_id = 0;  // Default to brick
    }
    return (const uint8_t*)pgm_read_ptr(&TEXTURE_ARRAY[texture_id]);
}

/**
 * Sample texture at coordinate with cyclic addressing
 * 
 * @param texture: Pointer to 32-byte texture in PROGMEM
 * @param texture_x: X coordinate (0-15, will wrap with & 15)
 * @param texture_y: Y coordinate (0-15, will wrap with & 15)
 * @return: 1 if pixel ON, 0 if OFF
 */
static inline uint8_t sampleTexture(const uint8_t* texture, uint8_t texture_x, uint8_t texture_y) {
    // Wrap coordinates
    texture_x &= (TEXTURE_SIZE - 1);  // & 15 = mod 16
    texture_y &= (TEXTURE_SIZE - 1);  // & 15 = mod 16
    
    // Calculate byte position: each row = 2 bytes
    uint8_t byte_offset = texture_y * 2 + (texture_x >> 3);  // x / 8
    uint8_t bit_offset = 7 - (texture_x & 7);  // 7 - (x mod 8)
    
    // Read byte from PROGMEM
    uint8_t byte_val = pgm_read_byte(&texture[byte_offset]);
    
    // Extract bit
    return (byte_val >> bit_offset) & 1;
}

}  // namespace DoomWallTextures

#endif  // DOOM_WALL_TEXTURES_H
