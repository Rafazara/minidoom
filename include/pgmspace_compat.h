/**
 * pgmspace_compat.h - Arduino PROGMEM compatibility shim for ESP32
 * 
 * The Doom Nano codebase was written for ATmega328P (Arduino Uno)
 * which uses avr/pgmspace.h for program memory annotations.
 * 
 * ESP32 doesn't have separate program memory like AVR, so PROGMEM
 * is defined in Arduino's pgmspace.h but it's compatible with GCC attributes.
 * 
 * This header provides a minimal compatibility layer.
 */

#ifndef PGMSPACE_COMPAT_H
#define PGMSPACE_COMPAT_H

#include <Arduino.h>

// On ESP32, PROGMEM is already defined by Arduino framework as:
// #define PROGMEM __attribute__((aligned(4)))
// 
// So we just need to ensure it's available

#ifndef PROGMEM
#define PROGMEM __attribute__((aligned(4)))
#endif

#endif // PGMSPACE_COMPAT_H
