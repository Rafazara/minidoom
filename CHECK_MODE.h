// This file is for checking which MODE is being compiled
#ifndef CHECK_MODE_H
#define CHECK_MODE_H

#ifdef MODE_DIAG
#error "COMPILING WITH MODE_DIAG"
#elif defined MODE_I2C_SCAN
#error "COMPILING WITH MODE_I2C_SCAN"
#elif defined MODE_OLED_TEST
#error "COMPILING WITH MODE_OLED_TEST"
#elif defined MODE_DOOM
#error "COMPILING WITH MODE_DOOM - CORRECT!"
#else
#error "NO MODE DEFINED - ERROR!"
#endif

#endif
