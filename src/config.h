#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// FIRMWARE MODES
// ============================================================================
// Selecione o modo via build_flags no platformio.ini:
// -D MODE_DIAG (padrão): diagnóstico do ESP32
// -D MODE_I2C_SCAN: varredura I2C
// -D MODE_OLED_TEST: teste completo com OLED

#ifndef MODE_DIAG
  #ifndef MODE_I2C_SCAN
    #ifndef MODE_OLED_TEST
      #define MODE_DIAG
    #endif
  #endif
#endif

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// I2C Pins for OLED SSD1306
#define OLED_SDA 21
#define OLED_SCL 22

// OLED Display Configuration
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_ADDRESS_PRIMARY 0x3C
#define OLED_ADDRESS_SECONDARY 0x3D

// ============================================================================
// DEBUG & LOGGING
// ============================================================================
#define LOG_PREFIX "[mini-doom] "
#define DEBUG_ENABLED 1

#if DEBUG_ENABLED
  #define LOG_DEBUG(fmt, ...) Serial.printf(LOG_PREFIX fmt "\r\n", ##__VA_ARGS__)
  #define LOG_INFO(fmt, ...) Serial.printf(LOG_PREFIX fmt "\r\n", ##__VA_ARGS__)
  #define LOG_ERROR(fmt, ...) Serial.printf(LOG_PREFIX "ERROR: " fmt "\r\n", ##__VA_ARGS__)
#else
  #define LOG_DEBUG(fmt, ...)
  #define LOG_INFO(fmt, ...)
  #define LOG_ERROR(fmt, ...)
#endif

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================
#define HEARTBEAT_INTERVAL 5000  // 5 segundos
#define I2C_SCAN_DELAY 500
#define OLED_INIT_TIMEOUT 2000

#endif // CONFIG_H
