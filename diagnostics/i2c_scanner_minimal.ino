#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n[I2C SCANNER] Starting I2C scan...");
  
  // Explicitly define pins
  const uint8_t SDA_PIN = 21;
  const uint8_t SCL_PIN = 22;
  
  Serial.print("[I2C] SDA=GPIO");
  Serial.print(SDA_PIN);
  Serial.print(" SCL=GPIO");
  Serial.println(SCL_PIN);
  
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  
  Serial.println("[I2C] Scanning addresses 0x00 to 0x7F...");
  
  uint8_t found_count = 0;
  
  for (uint8_t addr = 0x00; addr < 0x7F; addr++) {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("[I2C] Device FOUND at address: 0x");
      if (addr < 0x10) Serial.print("0");
      Serial.println(addr, HEX);
      found_count++;
    }
  }
  
  if (found_count == 0) {
    Serial.println("[I2C] NO DEVICES FOUND. Check wiring (SDA=GPIO21, SCL=GPIO22)");
  } else {
    Serial.print("[I2C] Total devices found: ");
    Serial.println(found_count);
  }
}

void loop() {
  delay(1000);
}
