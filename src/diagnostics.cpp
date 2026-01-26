#include "diagnostics.h"
#include "config.h"

namespace Diagnostics {

static unsigned long lastHeartbeat = 0;

void initialize() {
  LOG_INFO("ESP32 Diagnostics Module Initialized");
}

void printSystemInfo() {
  LOG_INFO("========== ESP32 SYSTEM INFO ==========");
  
  // CPU Info
  LOG_INFO("CPU Frequency: %d MHz", getCpuFrequencyMhz());
  LOG_INFO("CPU Cores: %d", ESP.getChipCores());
  
  // Memory Info
  LOG_INFO("Total Heap: %d bytes", ESP.getHeapSize());
  LOG_INFO("Free Heap: %d bytes", ESP.getFreeHeap());
  LOG_INFO("Used Heap: %d bytes", ESP.getHeapSize() - ESP.getFreeHeap());
  
  // Flash Info
  LOG_INFO("Flash Size: %d MB", ESP.getFlashChipSize() / (1024 * 1024));
  LOG_INFO("Flash Speed: %d MHz", ESP.getFlashChipSpeed() / 1000000);
  
  // Chip Info
  LOG_INFO("Chip Revision: %d", ESP.getChipRevision());
  LOG_INFO("SDK Version: %s", ESP.getSdkVersion());
  
  // Reset Reason
  LOG_INFO("Reset Reason: %s", Diagnostics::getResetReason());
  
  // Uptime
  LOG_INFO("Uptime: %lu ms", millis());
  
  LOG_INFO("=====================================");
}

void printHeartbeat() {
  unsigned long now = millis();
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    LOG_INFO("HEARTBEAT | Free Heap: %d bytes | Uptime: %lu ms", 
             ESP.getFreeHeap(), now);
  }
}

const char* getResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  
  switch (reason) {
    case ESP_RST_UNKNOWN:      return "Unknown";
    case ESP_RST_POWERON:      return "Power-On";
    case ESP_RST_EXT:          return "External Pin";
    case ESP_RST_SW:           return "Software";
    case ESP_RST_PANIC:        return "Panic";
    case ESP_RST_INT_WDT:      return "Interrupt WDT";
    case ESP_RST_TASK_WDT:     return "Task WDT";
    case ESP_RST_WDT:          return "Watchdog";
    case ESP_RST_DEEPSLEEP:    return "Deep Sleep";
    case ESP_RST_BROWNOUT:     return "Brownout";
    case ESP_RST_SDIO:         return "SDIO";
    default:                   return "Other";
  }
}

void runDiagnosticMode() {
  printSystemInfo();
  LOG_INFO("Running in DIAGNOSTIC MODE - printing heartbeat every %d ms", 
           HEARTBEAT_INTERVAL);
}

} // namespace Diagnostics
