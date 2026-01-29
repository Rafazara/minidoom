#ifndef DOOM_SPRITE_TEXTURES_H
#define DOOM_SPRITE_TEXTURES_H

#include <stdint.h>
#include <pgmspace.h>

/**
 * Doom-Style Sprite Textures - Phase 3.2
 * 
 * 16×16 pixel monochrome sprites for enemies and weapon
 * Each sprite = 32 bytes in PROGMEM (row-major format)
 * Row-major: each row = 2 bytes (16 pixels packed as bits)
 * 
 * Bit 0 = pixel ON (white), Bit 1 = pixel OFF (black)
 */

namespace DoomSpriteTextures {

constexpr uint8_t SPRITE_SIZE = 16;

// ============================================================================
// ENEMY SPRITES (3 states)
// ============================================================================

/**
 * ENEMY_IDLE: Standing pose
 * Simple demoníaco/humanoide com cabeça triangular e corpo
 */
static const uint8_t ENEMY_IDLE[32] PROGMEM = {
    0b00011000, 0b00000000,  // Row 0:    ██        (head tip)
    0b00111100, 0b00000000,  // Row 1:   ████       (head)
    0b01111110, 0b00000000,  // Row 2:  ██████      (head wider)
    0b11000011, 0b00000000,  // Row 3: ██    ██     (eyes)
    0b11111111, 0b00000000,  // Row 4: ████████     (head bottom)
    0b01111110, 0b00000000,  // Row 5:  ██████      (neck)
    0b00111100, 0b00000000,  // Row 6:   ████       (shoulders)
    0b01111110, 0b00000000,  // Row 7:  ██████      (body top)
    0b11111111, 0b00000000,  // Row 8: ████████     (body middle)
    0b11111111, 0b00000000,  // Row 9: ████████     (body)
    0b01111110, 0b00000000,  // Row 10:  ██████     (body)
    0b00111100, 0b00000000,  // Row 11:   ████      (waist)
    0b01000010, 0b00000000,  // Row 12:  █    █     (legs start)
    0b01100110, 0b00000000,  // Row 13:  ██  ██     (legs)
    0b00110110, 0b00000000,  // Row 14:   ██ ██     (legs)
    0b00011000, 0b00000000,  // Row 15:    ██       (feet)
};

/**
 * ENEMY_HIT: Recoil/pain pose
 * Ligeiramente inclinado para indicar impacto
 */
static const uint8_t ENEMY_HIT[32] PROGMEM = {
    0b00001100, 0b00000000,  // Row 0:     ██       (head tilted)
    0b00011100, 0b00000000,  // Row 1:    ███       
    0b00111100, 0b00000000,  // Row 2:   ████       
    0b00110011, 0b00000000,  // Row 3:   ██  ██     (eyes different)
    0b01111110, 0b00000000,  // Row 4:  ██████      
    0b01111110, 0b00000000,  // Row 5:  ██████      (neck)
    0b00111100, 0b00000000,  // Row 6:   ████       
    0b01111110, 0b00000000,  // Row 7:  ██████      
    0b11111111, 0b00000000,  // Row 8: ████████     
    0b10111101, 0b00000000,  // Row 9: █ ████ █     (slight wobble)
    0b01111110, 0b00000000,  // Row 10:  ██████     
    0b00111100, 0b00000000,  // Row 11:   ████      
    0b01000010, 0b00000000,  // Row 12:  █    █     
    0b00110110, 0b00000000,  // Row 13:   ██ ██     (legs shifted)
    0b01100110, 0b00000000,  // Row 14:  ██  ██     
    0b00001100, 0b00000000,  // Row 15:     ██      
};

/**
 * ENEMY_DEAD: Fallen/death pose
 * Corpo caído (poderia ser mais criativo, mas simples é melhor para 1-bit)
 */
static const uint8_t ENEMY_DEAD[32] PROGMEM = {
    0b11111111, 0b00000000,  // Row 0: ████████     (head horizontal)
    0b11111111, 0b00000000,  // Row 1: ████████     
    0b11111111, 0b00000000,  // Row 2: ████████     
    0b00000000, 0b00000000,  // Row 3:              (gap)
    0b11110000, 0b00000000,  // Row 4: ████         (body)
    0b11110000, 0b00000000,  // Row 5: ████         
    0b11110000, 0b00000000,  // Row 6: ████         
    0b11110000, 0b00000000,  // Row 7: ████         
    0b11110000, 0b00000000,  // Row 8: ████         
    0b01111000, 0b00000000,  // Row 9:  ███ (limbs)
    0b00111100, 0b00000000,  // Row 10:   ███       
    0b00011110, 0b00000000,  // Row 11:    ███      
    0b00001111, 0b00000000,  // Row 12:     ████    
    0b00000111, 0b10000000,  // Row 13:      ███ █  
    0b00000011, 0b11000000,  // Row 14:       ████  
    0b00000001, 0b11100000,  // Row 15:        ███  
};

// ============================================================================
// WEAPON SPRITES (2 states)
// ============================================================================

/**
 * WEAPON_IDLE: Resting pose
 * Pistola/arma Doom-style em repouso
 */
static const uint8_t WEAPON_IDLE[32] PROGMEM = {
    0b00000000, 0b00000000,  // Row 0:                  (empty)
    0b00000000, 0b00000000,  // Row 1:                  
    0b00001100, 0b00000000,  // Row 2:     ██          (slide)
    0b00001100, 0b00000000,  // Row 3:     ██          
    0b00011110, 0b00000000,  // Row 4:    ████        (barrel)
    0b00011110, 0b00000000,  // Row 5:    ████        
    0b00111111, 0b00000000,  // Row 6:   ██████       (wider)
    0b01111111, 0b10000000,  // Row 7:  ███████       (handle wide)
    0b01111111, 0b10000000,  // Row 8:  ███████       
    0b00111111, 0b00000000,  // Row 9:   ██████       
    0b00011110, 0b00000000,  // Row 10:    ████       
    0b00011110, 0b00000000,  // Row 11:    ████       
    0b00001100, 0b00000000,  // Row 12:     ██        
    0b00001100, 0b00000000,  // Row 13:     ██        
    0b00000000, 0b00000000,  // Row 14:                
    0b00000000, 0b00000000,  // Row 15:                
};

/**
 * WEAPON_FIRE: Firing pose
 * Arma com recuo (puxada mais para trás/lado)
 */
static const uint8_t WEAPON_FIRE[32] PROGMEM = {
    0b00000000, 0b00000000,  // Row 0:                  (empty)
    0b00000011, 0b00000000,  // Row 1:           ██    (muzzle flash)
    0b00000111, 0b10000000,  // Row 2:          ███    (muzzle)
    0b00000011, 0b00000000,  // Row 3:           ██    
    0b00000110, 0b00000000,  // Row 4:          ██     (slide pulled back)
    0b00000110, 0b00000000,  // Row 5:          ██     
    0b00001111, 0b00000000,  // Row 6:     ████        (barrel retracted)
    0b00111111, 0b10000000,  // Row 7:   ███████       (handle)
    0b00111111, 0b10000000,  // Row 8:   ███████       
    0b00001111, 0b00000000,  // Row 9:     ████        
    0b00000110, 0b00000000,  // Row 10:          ██    
    0b00000110, 0b00000000,  // Row 11:          ██    
    0b00000000, 0b00000000,  // Row 12:                
    0b00000000, 0b00000000,  // Row 13:                
    0b00000000, 0b00000000,  // Row 14:                
    0b00000000, 0b00000000,  // Row 15:                
};

// ============================================================================
// Texture Accessors
// ============================================================================

/**
 * Get enemy sprite by state
 * @param state: 0=ALIVE/IDLE, 1=HIT, 2=DEAD
 * @return Pointer to sprite texture in PROGMEM
 */
static inline const uint8_t* getEnemySprite(uint8_t state) {
    switch (state) {
        case 0:  // ENEMY_ALIVE
            return ENEMY_IDLE;
        case 1:  // ENEMY_HIT
            return ENEMY_HIT;
        case 2:  // ENEMY_DEAD
            return ENEMY_DEAD;
        default:
            return ENEMY_IDLE;
    }
}

/**
 * Get weapon sprite by state
 * @param firing: true = firing pose, false = idle pose
 * @return Pointer to sprite texture in PROGMEM
 */
static inline const uint8_t* getWeaponSprite(bool firing) {
    return firing ? WEAPON_FIRE : WEAPON_IDLE;
}

/**
 * Sample texture at coordinate
 * @param texture: Pointer to 32-byte sprite in PROGMEM
 * @param texture_x: X coordinate (0-15, will wrap)
 * @param texture_y: Y coordinate (0-15, will wrap)
 * @return: 1 if pixel ON, 0 if OFF
 */
static inline uint8_t sampleSprite(const uint8_t* texture, uint8_t texture_x, uint8_t texture_y) {
    // Wrap coordinates
    texture_x &= (SPRITE_SIZE - 1);  // & 15 = mod 16
    texture_y &= (SPRITE_SIZE - 1);  // & 15 = mod 16
    
    // Calculate byte position: each row = 2 bytes
    uint8_t byte_offset = texture_y * 2 + (texture_x >> 3);  // y*2 + x/8
    uint8_t bit_offset = 7 - (texture_x & 7);                // 7 - (x mod 8)
    
    // Read byte from PROGMEM
    uint8_t byte_val = pgm_read_byte(&texture[byte_offset]);
    
    // Extract bit
    return (byte_val >> bit_offset) & 1;
}

}  // namespace DoomSpriteTextures

#endif  // DOOM_SPRITE_TEXTURES_H
