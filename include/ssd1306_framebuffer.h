#ifndef SSD1306_FRAMEBUFFER_H
#define SSD1306_FRAMEBUFFER_H

#include <stdint.h>

namespace SSD1306Framebuffer {

constexpr uint8_t WIDTH = 128;
constexpr uint8_t HEIGHT = 64;
constexpr uint16_t SIZE = WIDTH * HEIGHT / 8;

inline uint16_t byteIndex(uint8_t x, uint8_t y) {
    return (y / 8) * WIDTH + x;
}

inline uint8_t bitMask(uint8_t y) {
    return (uint8_t)(1U << (y & 7));
}

inline void setPixel(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t color) {
    if (!buffer || x >= WIDTH || y >= HEIGHT) return;

    uint8_t mask = bitMask(y);
    if (color) {
        buffer[byteIndex(x, y)] |= mask;
    } else {
        buffer[byteIndex(x, y)] &= (uint8_t)~mask;
    }
}

inline uint8_t getPixel(const uint8_t* buffer, uint8_t x, uint8_t y) {
    if (!buffer || x >= WIDTH || y >= HEIGHT) return 0;
    return (buffer[byteIndex(x, y)] & bitMask(y)) ? 1 : 0;
}

}  // namespace SSD1306Framebuffer

#endif
