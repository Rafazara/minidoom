#ifndef DOOM_NANO_LEVEL_H
#define DOOM_NANO_LEVEL_H

#include <stdint.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

namespace DoomNanoLevel {

constexpr uint8_t WIDTH = 64;
constexpr uint8_t HEIGHT = 57;
constexpr uint16_t SIZE = WIDTH * HEIGHT / 2;
constexpr uint8_t WALL = 0x0F;

inline uint8_t getBlock(const uint8_t* level, double x, double y) {
    if (!level || x < 0.0 || y < 0.0 || x >= WIDTH || y >= HEIGHT) {
        return WALL;
    }

    uint8_t cell_x = (uint8_t)x;
    uint8_t cell_y = (uint8_t)y;
    uint16_t cell_index = ((HEIGHT - 1 - cell_y) * WIDTH) + cell_x;
    uint16_t byte_index = cell_index / 2;
#ifdef __AVR__
    uint8_t data = pgm_read_byte(level + byte_index);
#else
    uint8_t data = level[byte_index];
#endif
    return ((cell_x & 1) == 0) ? (data & 0x0F) : ((data >> 4) & 0x0F);
}

}  // namespace DoomNanoLevel

#endif
