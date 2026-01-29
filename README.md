# mini-doom: ESP32 + OLED + Doom Nano Integration

**Objective:** Professional test, validation, and execution pipeline for Doom Nano on ESP32 before full integration.

---

## 📋 Project Architecture

### System Components

```
┌─────────────────────────────────────────────────────────┐
│                      mini-doom                          │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │  ESP32-DevKit (240MHz, 8MB Flash, 520KB SRAM)   │  │
│  └──────────────────────────────────────────────────┘  │
│         │                                               │
│         ├─ GPIO21/22 (I2C) ──────┐                      │
│         │                         ▼                      │
│         │                  ┌──────────────┐             │
│         │                  │ SSD1306 OLED │             │
│         │                  │ 128x64 pixels│             │
│         │                  │ 0x3C / 0x3D  │             │
│         │                  └──────────────┘             │
│         │                                               │
│         └─ UART0 (Serial) ──── USB (Debug/Monitor)     │
│                                                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Firmware Modules                                │  │
│  │  ─────────────────────────────────────────────  │  │
│  │  • Diagnostics   (CPU, RAM, Flash, Reset info) │  │
│  │  • I2C Tools     (Bus scanning, device detect)  │  │
│  │  • OLED UI       (Display rendering, status)    │  │
│  │  • Doom Engine   (Integration stub/ready)       │  │
│  └──────────────────────────────────────────────────┘  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### Hardware Configuration

| Component | Pin | Protocol | Address | Status |
|-----------|-----|----------|---------|--------|
| ESP32 SDA | GPIO 21 | I2C | N/A | ✓ Configured |
| ESP32 SCL | GPIO 22 | I2C | N/A | ✓ Configured |
| OLED SSD1306 | I2C | I2C | 0x3C/0x3D | ✓ Auto-detect |
| Serial UART0 | RX0/TX0 | UART | N/A | ✓ 115200 baud |

---

## 🔄 Operation Modes

The firmware supports **4 operation modes** controlled by `build_flags` in `platformio.ini`:

### Mode 1: MODE_DIAG (Diagnostics)

**Purpose:** System health monitoring and diagnostics.

**Behavior:**
- Boot: Display ESP32 system information
- Loop: Print heartbeat every 5 seconds
- Output: CPU MHz, cores, heap (total/free/used), flash, chip revision, SDK version, reset reason, uptime

**Build Flag:**
```ini
build_flags = -D MODE_DIAG
```

**Output Example:**
```
[mini-doom] ========================================
[mini-doom] Active Mode: DIAGNOSTIC
[mini-doom] ========== ESP32 SYSTEM INFO ==========
[mini-doom] CPU Frequency: 240 MHz
[mini-doom] CPU Cores: 2
[mini-doom] Total Heap: 298468 bytes
[mini-doom] Free Heap: 270892 bytes
[mini-doom] Flash Size: 4 MB
[mini-doom] Reset Reason: Power-On
[mini-doom] Running in DIAGNOSTIC MODE
[mini-doom] HEARTBEAT | Free Heap: 270892 bytes | Uptime: 5000 ms
```

---

### Mode 2: MODE_I2C_SCAN (I2C Bus Scanning)

**Purpose:** Discover and identify I2C devices on the bus.

**Behavior:**
- Boot: Initialize I2C bus (GPIO21/22)
- Loop: Scan addresses 0x01-0x7E every 500ms
- Output: Device addresses found, device count

**Build Flag:**
```ini
build_flags = -D MODE_I2C_SCAN
```

**Output Example:**
```
[mini-doom] ========================================
[mini-doom] Active Mode: I2C_SCAN
[mini-doom] I2C Bus Initialized (SDA=21, SCL=22)
[mini-doom] Running in I2C SCAN MODE
[mini-doom] Scanning I2C devices every 500 ms...
[mini-doom] Scanning I2C bus...
[mini-doom] Device found at 0x3C
[mini-doom] Total devices found: 1
```

---

### Mode 3: MODE_OLED_TEST (OLED Display Test)

**Purpose:** Full OLED display validation with live system status.

**Behavior:**
- Boot: Auto-detect OLED address (0x3C primary, 0x3D secondary)
- Loop: Update display every 500ms with system status
- Fallback: If OLED not detected, automatically switch to MODE_DIAG

**Build Flag:**
```ini
build_flags = -D MODE_OLED_TEST
```

**Display Output:**
```
=== mini-doom ===

ESP32:  OK
OLED:   OK
I2C:    0x3C

Uptime: 42s
```

---

### Mode 4: MODE_DOOM (Doom Nano - Framebuffer Rendering)

**Purpose:** Doom Nano engine integration as pure renderer (Phase 2.1: Framebuffer only).

**Behavior:**
- Boot: Detect OLED, initialize graphics pipeline and Doom Nano adapter
- Loop: Non-blocking frame rendering at 20 FPS (50ms throttle)
- Output: [DOOM-CORE] initialization logs, continuous test pattern rendering
- Display: OLED shows Doom Nano framebuffer (test pattern in Phase 2.1)

**Build Flag:**
```ini
build_flags = -D MODE_DOOM
```

**Output Example:**
```
[mini-doom] ========================================
[mini-doom] Active Mode: DOOM (Graphics Pipeline - PURE RENDERING)
[mini-doom] ========================================

[DOOM-GFX] Initializing graphics system...
[DOOM-GFX] OLED detected at address: 0x3C
[DOOM-GFX] OLED initialized successfully

[DOOM-CORE] Initializing Doom Nano rendering engine...
[DOOM-CORE] Phase 2.1: Framebuffer rendering mode
[DOOM-CORE] [INFO] Doom Nano engine integrated as pure renderer

[DOOM-GFX] Rendering Doom Nano framebuffer
[DOOM-GFX] FPS target: 20 | Frame throttle: 50ms
```

**Status:** ✅ Phase 2.1 Complete - Ready for Phase 2.2 Engine Expansion
[mini-doom] ========================================
[mini-doom] Initializing Doom Nano...
[mini-doom] Doom: Allocating memory buffers...
[mini-doom] Doom: Frame buffer initialized
[mini-doom] Doom: Initialization complete ✓
[mini-doom] Running in DOOM MODE - Waiting for phase 2 integration
[mini-doom] Doom: Tick #1 | Frame: 1 | Free Heap: 265432 bytes
```

---

## 🚀 Quick Start

### Prerequisites

- **PlatformIO CLI** installed
- **ESP32-DevKit** board connected to **COM7** (configurable)
- **SSD1306 OLED** display (128x64) connected via I2C (GPIO21/22)
- **Python 3.x** (for pre-upload script)

### Installation & Building

1. **Clone/Open Project:**
   ```bash
   cd ~/mini-doom
   ```

2. **Build firmware:**
   ```bash
   pio run
   ```

3. **Upload to ESP32:**
   ```bash
   pio run -t upload
   ```

4. **Monitor serial output:**
   ```bash
   pio device monitor -b 115200
   ```

5. **Complete cycle (build + upload + monitor):**
   ```bash
   pio run -t upload && pio device monitor -b 115200
   ```

---

## 🧪 Testing Pipeline

### Test 1: System Diagnostics (MODE_DIAG)

**Objective:** Validate ESP32 hardware and system resources.

```bash
# Edit platformio.ini:
# build_flags = -D MODE_DIAG

pio run -t upload
pio device monitor -b 115200
```

**Expected Output:**
- CPU frequency (240 MHz)
- Available cores (2)
- Total heap size
- Free heap size
- Flash size (4 MB)
- Reset reason
- Heartbeat every 5 seconds

---

### Test 2: I2C Bus Scanning (MODE_I2C_SCAN)

**Objective:** Detect OLED and other I2C devices.

```bash
# Edit platformio.ini:
# build_flags = -D MODE_I2C_SCAN

pio run -t upload
pio device monitor -b 115200
```

**Expected Output:**
- I2C initialized on GPIO21/22
- Device found at address 0x3C or 0x3D (OLED)
- Scan repeats every 500ms

---

### Test 3: OLED Display Test (MODE_OLED_TEST)

**Objective:** Validate OLED rendering and live status display.

```bash
# Edit platformio.ini:
# build_flags = -D MODE_OLED_TEST

pio run -t upload
pio device monitor -b 115200
```

**Expected Output:**
- OLED auto-detection message
- Status screen on OLED display
- Uptime updates every 500ms

---

### Test 4: Doom Stub Integration (MODE_DOOM - Default)

**Objective:** Verify Doom engine stub is ready for Phase 2 integration.

```bash
# platformio.ini (default):
# build_flags = -D MODE_DOOM

pio run -t upload
pio device monitor -b 115200
```

**Expected Output:**
- Doom initialization messages
- Buffer allocation logs
- Frame counter incrementing every 1000ms
- Free heap status

---

## 📝 File Structure

```
mini-doom/
├── platformio.ini                 # Build configuration + mode selection
├── README.md                      # This file
│
├── src/
│   ├── main.cpp                  # Entry point + mode dispatcher
│   ├── config.h                  # Hardware pins + build modes
│   │
│   ├── diagnostics.cpp           # System diagnostics implementation
│   ├── diagnostics.h             # Diagnostics API
│   │
│   ├── i2c_tools.cpp             # I2C scanning + device detection
│   ├── i2c_tools.h               # I2C tools API
│   │
│   ├── oled_ui.cpp               # OLED rendering + status display
│   ├── oled_ui.h                 # OLED UI API
│   │
│   └── doom/
│       ├── doom_integration.cpp   # Doom stub implementation
│       └── doom_integration.h     # Doom API
│
├── scripts/
│   └── pre_upload.py             # Pre-upload hooks (port ready check)
│
└── third_party/
    └── doom-nano/                # Doom Nano source (Phase 2)
```

---

## 🔗 Boot Flow

```
POWER ON
   ↓
Serial Init (115200 baud)
   ↓
Print boot banner + active mode
   ↓
Module Initialization (Diagnostics, I2C)
   ↓
Mode Selection (build_flags):
   ├─ MODE_DIAG      → Diagnostics::runDiagnosticMode()
   ├─ MODE_I2C_SCAN  → I2CTools::runI2CScanMode()
   ├─ MODE_OLED_TEST → OLEDUI::displayStatus() loop
   └─ MODE_DOOM      → DoomIntegration::setup() → loop()
   ↓
Main Loop (mode-specific)
```

---

## ⚙️ Configuration

### platformio.ini

| Setting | Value | Purpose |
|---------|-------|---------|
| `platform` | `espressif32` | ESP32 platform |
| `board` | `esp32doit-devkit-v1` | Board type |
| `framework` | `arduino` | Arduino framework |
| `upload_port` | `COM7` | USB serial port |
| `monitor_port` | `COM7` | Serial monitor port |
| `monitor_speed` | `115200` | Serial baud rate |
| `upload_speed` | `460800` | Upload baud rate (fast) |
| `build_flags` | `-D MODE_DOOM` | Active operation mode |

### config.h

| Define | Value | Purpose |
|--------|-------|---------|
| `OLED_SDA` | `21` | I2C data pin |
| `OLED_SCL` | `22` | I2C clock pin |
| `OLED_WIDTH` | `128` | Display width (pixels) |
| `OLED_HEIGHT` | `64` | Display height (pixels) |
| `OLED_ADDRESS_PRIMARY` | `0x3C` | Default OLED address |
| `OLED_ADDRESS_SECONDARY` | `0x3D` | Alternate OLED address |
| `HEARTBEAT_INTERVAL` | `5000` | Diagnostics heartbeat (ms) |
| `I2C_SCAN_DELAY` | `500` | I2C scan interval (ms) |
| `OLED_INIT_TIMEOUT` | `2000` | OLED init timeout (ms) |

---

## 📊 System Status Report

### Hardware Status

| Component | Status | Details |
|-----------|--------|---------|
| **ESP32** | ✓ OK | 240 MHz, 2 cores, 520KB SRAM |
| **Flash** | ✓ OK | 4 MB available |
| **I2C Bus** | ✓ OK | GPIO21/22, 100kHz |
| **OLED Display** | ✓ OK | SSD1306, 128x64, 0x3C auto-detect |
| **Serial Monitor** | ✓ OK | COM7, 115200 baud |

### Memory Status

| Type | Size | Status |
|------|------|--------|
| **Total SRAM** | 520 KB | ✓ Sufficient |
| **Free Heap (Boot)** | ~270 KB | ✓ Adequate |
| **Doom Buffer** | 1 KB | ✓ Reserved |
| **Margin** | ~269 KB | ✓ Safe |

### Feature Status

| Feature | Status | Notes |
|---------|--------|-------|
| **MODE_DIAG** | ✓ Ready | System diagnostics |
| **MODE_I2C_SCAN** | ✓ Ready | Device detection |
| **MODE_OLED_TEST** | ✓ Ready | Display validation |
| **MODE_DOOM** | ✓ Ready | Stub, Phase 2 pending |
| **Pre-Upload Script** | ✓ Ready | Port management |

---

## 🛠️ Troubleshooting

### OLED Not Detected

**Symptoms:** "OLED not detected" message

**Solutions:**
1. Check I2C connections (GPIO21/22)
2. Run `MODE_I2C_SCAN` to verify I2C communication
3. Check OLED power supply (3.3V)

### Upload Fails

**Symptoms:** Serial port error, upload cannot start

**Solutions:**
1. Verify `upload_port = COM7` in `platformio.ini`
2. Check physical USB connection
3. Close any serial monitor before uploading
4. Reset ESP32 (RESET button)

### Watchdog Reset

**Symptoms:** Frequent resets, "Task WDT" messages

**Solutions:**
1. Check for infinite loops
2. Profile with `MODE_DIAG`
3. Ensure all modules initialize correctly

---

## 📚 References

- [ESP32 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)
- [Doom Nano Source](https://github.com/daveruiz/doom-nano)

---

## 📄 License & Notes

- **Project:** mini-doom (Doom engine on ESP32)
- **Target:** Doom Nano integration on ESP32+OLED
- **Status:** Phase 1 (Testing & Validation) ✓ Complete
- **Next Phase:** Phase 2 (Doom Nano Engine Integration)

---

**Last Updated:** January 27, 2026  
**Version:** 1.0 - Testing & Validation Pipeline Complete


