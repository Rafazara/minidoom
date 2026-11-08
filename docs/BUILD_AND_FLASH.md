# Build and flash

## Reproducible configuration

`platformio.ini` defines three environments:

| Environment | Purpose |
|---|---|
| `esp32doit-devkit-v1` | Normal game firmware; default |
| `esp32doit-devkit-v1-oled-diag` | Isolated OLED patterns; no game loop |
| `native` | Host-side framebuffer and map-decoding tests |

Firmware dependency versions are pinned to the versions successfully compiled during recovery:

- `espressif32 @ 6.12.0`
- `Adafruit GFX Library @ 1.13.0`
- `Adafruit SSD1306 @ 2.5.17`
- Adafruit BusIO resolved transitively as 1.17.4 during the recorded build

The Doom Nano submodule is pinned by the superproject. Initialize it after cloning:

```powershell
git submodule update --init --recursive
git submodule status
```

## Build

From the repository root:

```powershell
pio run -e esp32doit-devkit-v1
```

For a clean rebuild:

```powershell
pio run -e esp32doit-devkit-v1 -t clean
pio run -e esp32doit-devkit-v1
```

A successful result includes `SUCCESS`, RAM use, flash use, and `firmware.bin`. It does not establish hardware functionality or frame rate.

## Upload

Upload changes the physical device and must be intentional. Confirm the port in `platformio.ini`, close other monitor programs, then run:

```powershell
pio run -e esp32doit-devkit-v1 -t upload
```

The configured upload speed is 460800 baud. If a particular cable/board combination is unreliable, record the failure before changing that setting.

## Serial monitor

```powershell
pio device monitor -p COM7 -b 115200
```

Use the actual assigned port. Save the boot log and any error messages with hardware test results.

## OLED diagnostic

Build without uploading:

```powershell
pio run -e esp32doit-devkit-v1-oled-diag
```

After approval to flash:

```powershell
pio run -e esp32doit-devkit-v1-oled-diag -t upload
```

Return to normal firmware:

```powershell
pio run -e esp32doit-devkit-v1 -t upload
```

The diagnostic environment does not require editing `build_flags` and does not enable `VALIDATION_MODE`.

## Host tests

```powershell
pio test -e native
```

A host GCC/G++ toolchain must be discoverable by PlatformIO. On the recovery machine, test discovery succeeded but compilation stopped because `gcc` and `g++` were unavailable. No test pass is claimed.

## PlatformIO/Python environment findings

The recorded host resolves one executable through PATH:

```text
C:\Users\rafae\AppData\Local\Programs\Python\Python315\Scripts\pio.exe
```

It uses Python 3.15.0a2 and PlatformIO Core 6.1.18. PlatformIO prints an obsolete-core notice referring to a previous 6.2.0 installation, and `requests` prints a dependency-compatibility warning involving urllib3 and character-detection packages.

These warnings did not prevent either firmware environment from compiling. No global package was upgraded, removed, or downgraded. Resolving duplicate core state or Python package compatibility should be done separately with a supported stable Python and, preferably, a project-local virtual environment; it is not required to reproduce the recorded successful build on this machine.

## Reproducibility checklist

1. Use the intended worktree and branch; do not confuse `master` with the OLED recovery branch.
2. Confirm the Doom Nano submodule matches the recorded gitlink.
3. Run the default and diagnostic builds.
4. Record RAM, flash, compiler warnings, and tool versions.
5. Run host tests when a compiler is available.
6. Treat upload, visual display behavior, controls, and FPS as separate hardware-validation steps.
