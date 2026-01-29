# 🚀 mini-doom - QUICK START GUIDE

**Status:** ✅ Phase 1 Complete (Testing & Validation Pipeline)  
**Ready for:** Doom Nano Engine Integration (Phase 2)

---

## ⚡ 30-Second Summary

The **mini-doom** project is now a fully validated, professional ESP32 firmware with:
- ✅ 4 independent test modes
- ✅ I2C/OLED auto-detection
- ✅ Professional build pipeline
- ✅ Comprehensive documentation
- ✅ Ready for Doom Nano integration

---

## 🎯 What's Working Right Now

### MODE_DIAG (Diagnostics)
See CPU, RAM, Flash, reset reason, heartbeat every 5s

### MODE_I2C_SCAN (I2C Discovery)
Scan I2C bus (0x01-0x7E), find OLED at 0x3C or 0x3D

### MODE_OLED_TEST (Display Test)
Live status on OLED display (uptime, heap, I2C address)

### MODE_DOOM (Stub Ready)
Doom engine initialization placeholder, frame counter, ready for Phase 2

---

## 🚀 Quick Commands

```bash
# Build firmware
pio run

# Upload to ESP32
pio run -t upload

# Monitor serial output
pio device monitor -b 115200

# Full cycle (build + upload + monitor)
pio run -t upload && pio device monitor -b 115200
```

---

## 📋 How to Change Modes

Edit `platformio.ini` and change the `build_flags` line:

```ini
# For Diagnostics
build_flags = -D MODE_DIAG

# For I2C Scanning
build_flags = -D MODE_I2C_SCAN

# For OLED Display Test
build_flags = -D MODE_OLED_TEST

# For Doom Stub (default)
build_flags = -D MODE_DOOM
```

Then run: `pio run -t upload && pio device monitor -b 115200`

---

## 📚 Documentation

| File | Purpose |
|------|---------|
| **README.md** | Complete guide (architecture, modes, troubleshooting) |
| **IMPLEMENTATION_REPORT.md** | Technical summary |
| **platformio.ini** | Build config with mode comments |
| **config.h** | Hardware pins & settings |

---

## 🔌 Hardware Verified

| Component | Status | Details |
|-----------|--------|---------|
| ESP32 | ✓ OK | 240 MHz, 520KB SRAM |
| Flash | ✓ OK | 4 MB (77% free) |
| I2C | ✓ OK | GPIO21/22 |
| OLED | ✓ OK | 0x3C/0x3D auto-detect |
| Serial | ✓ OK | COM7, 115200 baud |

---

## 📊 Memory Status

| Resource | Used | Free | Status |
|----------|------|------|--------|
| **Flash** | 22.7% | 77.3% | ✅ Excellent |
| **RAM** | 6.7% | 93.3% | ✅ Excellent |

---

## ✨ What's New

### Phase 1 Deliverables
1. ✅ 4 operation modes (DIAG, I2C_SCAN, OLED_TEST, DOOM)
2. ✅ Enhanced pre_upload.py script with mode detection
3. ✅ Professional README.md (450+ lines)
4. ✅ Implementation report with all technical details
5. ✅ Compilation successful (0 errors, 0 warnings)
6. ✅ Hardware fully validated
7. ✅ Memory profile optimized
8. ✅ Ready for Phase 2 (Doom Nano integration)

---

## 🎮 Phase 2 Roadmap

| Task | Status | Notes |
|------|--------|-------|
| Port doom-nano engine | ⏳ Pending | In third_party/ |
| Design rendering adapter | ⏳ Pending | 128x64 OLED |
| Implement input handling | ⏳ Pending | Button mapping |
| Optimize memory | ⏳ Pending | 305 KB available |
| Profile performance | ⏳ Pending | Target 30+ FPS |

---

## 🛠️ Troubleshooting

### OLED not detected?
→ Run `MODE_I2C_SCAN` to verify I2C communication

### Serial gibberish?
→ Check monitor_speed = 115200 in platformio.ini

### Upload fails?
→ Close serial monitor, verify COM7, reset ESP32

### Watchdog reset?
→ Profile with MODE_DIAG to check memory and loops

---

## 📖 Full Documentation

For detailed information, see:
- **[README.md](README.md)** - Complete project guide
- **[IMPLEMENTATION_REPORT.md](IMPLEMENTATION_REPORT.md)** - Technical summary

---

## ✅ System Status Report

```
[mini-doom] SYSTEM STATUS
━━━━━━━━━━━━━━━━━━━━━━━━
CPU:                    ✓ OK (240 MHz)
FLASH:                  ✓ OK (77% free)
RAM:                    ✓ OK (93% free)
I2C BUS:                ✓ OK (GPIO21/22)
OLED DETECTION:         ✓ OK (0x3C/0x3D)
GRAPHICS PIPELINE:      ✓ READY (stub)
DOOM STUB:              ✓ OPERATIONAL
━━━━━━━━━━━━━━━━━━━━━━━━
READY FOR DOOM ENGINE:  YES ✓
```

---

## 🏆 Status: Phase 1 ✅ COMPLETE

**Project fully validated and ready for production testing and Doom Nano integration.**

---

**Last Updated:** January 27, 2026  
**Version:** 1.0 - Testing & Validation Pipeline Complete

