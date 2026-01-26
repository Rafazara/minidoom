# mini-doom: Doom Nano on ESP32 with OLED

Professional embedded firmware project for running Doom Nano on ESP32 DevKit V1 with SSD1306 I2C OLED display.

## Hardware Specifications

| Component | Details |
|-----------|---------|
| **Microcontroller** | ESP32 DevKit V1 |
| **Display** | OLED SSD1306 128x64 I2C |
| **Serial Monitor** | 115200 baud |
| **Framework** | Arduino (PlatformIO) |

## OLED Wiring (I2C)

Connect the OLED SSD1306 display to the ESP32 as follows:

```
OLED Pin  →  ESP32 Pin
─────────────────────
VCC       →  3V3
GND       →  GND
SDA       →  GPIO 21
SCL       →  GPIO 22
```

**Note:** The I2C address is automatically detected (0x3C or 0x3D).

## Firmware Modes

The firmware supports 4 selectable modes via build flags:

### 1. **MODE_DIAG** (Default)
Comprehensive ESP32 diagnostic information:
- CPU frequency and core count
- Memory usage (heap size, free heap)
- Flash memory info
- Reset reason
- Uptime and heartbeat (every 5s)

### 2. **MODE_I2C_SCAN**
I2C bus scanner:
- Scans all I2C addresses (0x01-0x7E)
- Reports found devices
- Runs every 500ms

### 3. **MODE_OLED_TEST**
Complete system test with OLED display:
- Detects OLED I2C address automatically
- Initializes display
- Shows real-time system status (ESP32 OK, OLED OK, I2C addr, uptime)
- Falls back to diagnostic mode if OLED fails

### 4. **MODE_DOOM** (Stub/Integration Phase)
Doom Nano integration stub:
- Initializes Doom engine memory and buffers
- Prints tick/frame information
- Currently: **NO rendering yet** (placeholder for Phase 2)
- Logs: Memory allocation, frame count, heap usage
- **Status:** Phase 1 ✓ (Integration stub), Phase 2 ⏳ (Rendering adapter)

## How to Change Mode

Edit `platformio.ini` and modify the `build_flags` line:

```ini
build_flags = 
    -D MODE_DIAG        ; Diagnostic mode (default)
    ; -D MODE_I2C_SCAN  ; Uncomment for I2C scanning
    ; -D MODE_OLED_TEST ; Uncomment for OLED test
    ; -D MODE_DOOM      ; Uncomment for Doom Nano integration (stub)
```

## Building and Uploading

### Requirements
- PlatformIO CLI or VS Code + PlatformIO extension
- Python 3.6+ (PlatformIO dependency)

### VS Code Tasks (Recommended)

Open Command Palette (`Ctrl+Shift+P`) → `Tasks: Run Task`

| Task | Purpose |
|------|---------|
| **PIO: Build Only** | Compile only (no upload) |
| **PIO: Upload Safe** | Compile + **auto-cleanup COM port** + upload ✅ |
| **PIO: Monitor** | Start serial monitor (115200 baud) |

### Command Line

**Compile**
```bash
pio run
```

**Upload (with automatic COM port cleanup)**
```bash
pio run -t upload
```

**Monitor Serial Output**
```bash
pio device monitor -b 115200
```

**Combined (compile → upload → monitor)**
```bash
pio run -t upload && pio device monitor -b 115200
```

## Project Structure

```
mini-doom/
├── .git/                   # Git repository (version control)
├── .gitignore              # Git ignore rules (PlatformIO artifacts)
├── .gitmodules             # Git submodule configuration
├── platformio.ini          # PlatformIO configuration
├── README.md               # This file
│
├── scripts/
│   └── pre_upload.py       # Pre-upload hook (COM port cleanup)
│
├── src/
│   ├── main.cpp            # Firmware entry point (setup/loop)
│   ├── config.h            # Global configuration & macros
│   ├── diagnostics.h/.cpp  # ESP32 diagnostic utilities
│   ├── i2c_tools.h/.cpp    # I2C bus scanning & OLED detection
│   └── oled_ui.h/.cpp      # OLED display rendering
│
├── include/                # Optional: header files
├── lib/                    # Optional: custom libraries
├── test/                   # Optional: test files
│
├── third_party/
│   └── doom-nano/          # Doom Nano engine (as Git submodule)
│       ├── .git/
│       ├── doom-nano.ino
│       ├── entities.h/.cpp
│       ├── input.h/.cpp
│       └── ... (19 files)
│
└── .vscode/
    ├── tasks.json          # VS Code build/upload/monitor tasks
    ├── launch.json         # Debug configuration
    ├── c_cpp_properties.json
    └── extensions.json
```

## Key Features

✅ **Git Version Control** - Full Git repository with submodule support  
✅ **Submodule Integration** - Doom Nano as Git submodule (automatic updates)  
✅ **Robust Error Handling** - Falls back gracefully if OLED fails  
✅ **Minimal Overhead** - No blocking operations (except scan mode)  
✅ **Professional Logging** - Prefixed, clear debug output  
✅ **Modular Design** - Easy to extend for Doom Nano integration  
✅ **I2C Auto-Detection** - Automatically finds OLED at 0x3C or 0x3D  
✅ **Zero OLED Dependency** - Runs flawlessly without display  
✅ **Pre-Upload Hook** - Automatic COM port cleanup before upload  

## Dependencies

- **Adafruit GFX Library** (v1.11.9+) - Graphics library
- **Adafruit SSD1306** (v2.5.9+) - OLED driver

Automatically installed via PlatformIO.

## Troubleshooting

### OLED not detected
1. Verify wiring (SDA=GPIO21, SCL=GPIO22)
2. Check OLED power (should show blue LED if powered)
3. Run `MODE_I2C_SCAN` to verify I2C communication
4. Try both I2C addresses: 0x3C and 0x3D

### Serial monitor shows gibberish
- Ensure monitor_speed is 115200 in platformio.ini
- Check USB cable connection
- Try different USB port

### Compilation errors
- Run `pio lib update` to refresh dependencies
- Ensure ESP32 platform is installed: `pio platform install espressif32`

### COM Port Error: "Could not open COM7, port is busy or doesn't exist" (PermissionError 13)

**Cause:** Serial monitor or another process is holding the COM port.

**Quick Fixes (Try in order):**

1. **Close the Serial Monitor**
   - Close VS Code's serial monitor tab
   - Stop any other serial terminal (PuTTY, Minicom, etc.)

2. **Use "PIO: Upload Safe" Task** (Recommended)
   - Open Command Palette: `Ctrl+Shift+P`
   - Type: `Tasks: Run Task`
   - Select: `PIO: Upload Safe`
   - This automatically kills processes holding the COM port

3. **Manual Port Release on Windows**
   ```powershell
   # Close PlatformIO processes
   taskkill /F /IM python.exe
   taskkill /F /IM platformio.exe
   
   # Wait a moment
   Start-Sleep -Seconds 1
   
   # Retry upload
   pio run -t upload
   ```

4. **Physical USB Disconnect/Reconnect**
   - Unplug ESP32 from USB
   - Wait 2 seconds
   - Plug back in
   - Retry upload

5. **Restart VS Code**
   - Close and reopen VS Code completely
   - This releases all resources

**Prevention:**
- Always close the Monitor before uploading
- Use `PIO: Upload Safe` task (includes automatic cleanup)
- Keep `scripts/pre_upload.py` enabled in `platformio.ini` for automatic port release

## Future Work: Doom Nano Integration

The `third_party/doom-nano/` folder contains the **Doom Nano engine** as a Git submodule.

### Cloning with Submodules

```bash
# Clone repository including submodules
git clone --recursive https://your-repo.git

# Or, if already cloned, initialize submodules
git submodule update --init --recursive
```

### Updating Doom Nano Submodule

```bash
cd third_party/doom-nano
git pull origin master
cd ../..
git add third_party/doom-nano
git commit -m "update doom-nano to latest version"
```

### Integration Roadmap

Expected steps:
1. Port Doom Nano source code into firmware
2. Adapt rendering pipeline to 128x64 OLED constraints
3. Implement input handling (button mapping to GPIO pins)
4. Optimize memory usage for ESP32 (~520KB available heap)
5. Performance tuning (target 30 FPS)

## License

This project is provided as-is for educational and embedded systems development.

---

**Created:** January 2026  
**Platform:** ESP32 DevKit V1  
**Framework:** Arduino + PlatformIO
