#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// FIRMWARE MODES
// ============================================================================
// Selecione o modo via build_flags no platformio.ini:
// -D MODE_DIAG (padrão): diagnóstico do ESP32
// -D MODE_I2C_SCAN: varredura I2C
// -D MODE_OLED_TEST: teste completo com OLED
// -D MODE_DOOM: pipeline gráfico (renderização isolada)

#ifndef MODE_DIAG
  #ifndef MODE_I2C_SCAN
    #ifndef MODE_OLED_TEST
      #ifndef MODE_DOOM
        #define MODE_DIAG
      #endif
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
#define OLED_HEIGHT 32
#define OLED_ADDRESS_PRIMARY 0x3C
#define OLED_ADDRESS_SECONDARY 0x3D

// ============================================================================
// DEBUG & LOGGING (Phase 4.0: Release Candidate)
// ============================================================================
// MINI_DOOM_DEBUG: Set to 1 for debug logs, 0 for clean release build
// Default: 0 (RC mode - clean output)
#define MINI_DOOM_DEBUG 0

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

// Phase 4.0: Debug-only macros for balance/flow/AI logging
#if MINI_DOOM_DEBUG
  #define LOG_BALANCE(fmt, ...) Serial.printf("[BALANCE] " fmt "\r\n", ##__VA_ARGS__)
  #define LOG_FLOW(fmt, ...) Serial.printf("[FLOW] " fmt "\r\n", ##__VA_ARGS__)
  #define LOG_AI(fmt, ...) Serial.printf("[AI] " fmt "\r\n", ##__VA_ARGS__)
#else
  #define LOG_BALANCE(fmt, ...)
  #define LOG_FLOW(fmt, ...)
  #define LOG_AI(fmt, ...)
#endif

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================
#define HEARTBEAT_INTERVAL 5000  // 5 segundos
#define I2C_SCAN_DELAY 500
#define OLED_INIT_TIMEOUT 2000

#endif // CONFIG_H
