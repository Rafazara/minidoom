/**
 * I2C SSD1306 Address Scanner - Standalone Diagnostic
 * For ESP32 with SSD1306 OLED on GPIO21 (SDA), GPIO22 (SCL)
 * 
 * Purpose: Verify I2C connectivity and detect SSD1306 address
 * Safe to run independently of game code
 * 
 * Usage:
 * 1. Upload this sketch to ESP32
 * 2. Open Serial Monitor at 115200 baud
 * 3. Read detected addresses
 * 4. Compare with OLED_ADDRESS_PRIMARY/SECONDARY in config.h
 */

#include <Wire.h>

// ESP32 I2C pins
#define I2C_SDA 21
#define I2C_SCL 22

// OLED bus configuration
#define I2C_CLOCK 100000  // 100 kHz - safe for SSD1306

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n");
    Serial.println("========================================");
    Serial.println("I2C SSD1306 Address Scanner (ESP32)");
    Serial.println("========================================");
    Serial.printf("SDA: GPIO %d\n", I2C_SDA);
    Serial.printf("SCL: GPIO %d\n", I2C_SCL);
    Serial.printf("I2C Clock: %d Hz\n", I2C_CLOCK);
    Serial.println("");
    Serial.println("Initializing I2C bus...");
    
    // Initialize I2C with safe speed
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_CLOCK);
    
    // Power stabilization delay (critical for SSD1306)
    delay(100);
    
    Serial.println("✓ I2C bus ready");
    Serial.println("Scanning address range 0x03–0x77...");
    Serial.println("");
    
    uint8_t device_count = 0;
    
    // Scan full I2C range
    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        Wire.beginTransmission(addr);
        uint8_t result = Wire.endTransmission();
        
        if (result == 0) {
            // Device found
            Serial.printf("✓ Found device at 0x%02X (%3d)\n", addr, addr);
            device_count++;
            
            // Highlight likely OLED addresses
            if (addr == 0x3C || addr == 0x3D) {
                Serial.println("  → Typical SSD1306 OLED address");
            }
        }
    }
    
    Serial.println("");
    Serial.println("========================================");
    
    if (device_count == 0) {
        Serial.println("✗ NO DEVICES FOUND");
        Serial.println("");
        Serial.println("Troubleshooting:");
        Serial.println("  1. Check OLED power supply (3.3V VCC)");
        Serial.println("  2. Verify GND connection");
        Serial.println("  3. Check SDA/SCL wiring (GPIO21/GPIO22)");
        Serial.println("  4. Look for pull-up resistors (4.7kΩ typical)");
        Serial.println("  5. Try swapping SDA/SCL if needed");
    } else {
        Serial.printf("✓ Scan complete: %d device(s) found\n", device_count);
        Serial.println("");
        Serial.println("For SSD1306 OLED, typical addresses are:");
        Serial.println("  • 0x3C (default, most common)");
        Serial.println("  • 0x3D (alternate, some boards)");
        Serial.println("");
        Serial.println("Update config.h if address differs:");
        Serial.println("  #define OLED_ADDRESS_PRIMARY 0x3C");
    }
    
    Serial.println("========================================");
    Serial.println("");
}

void loop() {
    // Idle after scan
    // To re-scan, press RST button on ESP32
    delay(5000);
}
