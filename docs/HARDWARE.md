# Hardware

## Confirmed baseline

| Item | Configuration | Evidence |
|---|---|---|
| Board | ESP32 DOIT DevKit V1 | PlatformIO board configuration |
| Chip | ESP32-D0WDQ6 | Observed during a previous upload |
| CPU | Dual-core Xtensa, configured up to 240 MHz | Board/platform metadata |
| Physical flash | 4 MB | Previous PlatformIO upload report |
| OLED | 128×64, I²C, SSD1306-compatible | Prior physical tests; exact controller not proven |
| OLED address | `0x3C` | Previous physical I²C scan |
| SDA / SCL | GPIO21 / GPIO22 | Source configuration and known wiring |
| Serial | 115200 baud; COM7 previously successful | Firmware configuration and local history |

An I²C response establishes bus presence, not that the module contains a genuine SSD1306 or that its full framebuffer works. An older report suspected a bicolor clone; the currently attached module must be identified and tested rather than assumed replaced.

## Wiring and breadboard arrangement

| OLED pin | ESP32 pin | Purpose |
|---|---|---|
| GND | GND | Common ground |
| VCC | 3.3V | Display-module supply |
| SCL / SCK | GPIO22 | I²C clock |
| SDA | GPIO21 | I²C data |

```text
                 USB data and power
                       |
                +------v-------+
                | ESP32 DevKit |
                |              |
        3.3V ---+ 3.3V     GND +--- GND
                |              |
       GPIO22 --+ SCL      SDA +-- GPIO21
                +------+-------+
                       |
                +------v-------+
                | OLED 128x64  |
                | VCC GND      |
                | SCL SDA      |
                +--------------+
```

Place the OLED beside the ESP32 with short, secure connections. Keep SDA/SCL away from loose power jumpers. The diagram is logical wiring, not a claim that a particular breadboard assembly has been completed.

## Power and safety

- Use the known-compatible 3.3 V connection for this OLED module. Check the module manufacturer's limits before using any other supply.
- ESP32 GPIO is 3.3 V logic and is not 5 V tolerant.
- Always connect OLED GND and ESP32 GND.
- Disconnect USB power before rewiring. Avoid shorts between 3.3 V, GND, and GPIO.
- A USB port or suitable regulated USB supply powers the development baseline. Do not exceed board or regulator ratings.
- Do not connect a bare lithium cell directly to the ESP32 or OLED. Any future battery design needs a suitable charger, protection, regulator, and a defined power path.

## Expected startup behavior

For the recovered normal firmware, the intended sequence is:

1. Serial starts at 115200 baud.
2. I²C starts on GPIO21/22 at 100 kHz.
3. Adafruit SSD1306 initializes at `0x3C` with a 128×64 buffer.
4. Graphics and game modules initialize.
5. The title framebuffer is generated and transferred to the OLED.
6. A confirmed optional Fire input starts gameplay.

This sequence is software-verified only. The physical title and display stability remain pending.

## Build, upload, and monitor

```powershell
pio run -e esp32doit-devkit-v1
pio run -e esp32doit-devkit-v1 -t upload
pio device monitor -p COM7 -b 115200
```

Change COM7 if Windows assigns another port. Uploading requires explicit user approval when performed by the development agent.

## Troubleshooting

| Symptom | Checks |
|---|---|
| No serial output | Confirm COM port, 115200 baud, USB data cable, and driver |
| Upload fails | Close monitor programs; verify port; use RESET/BOOT only if the board requires it |
| OLED init failure | Verify address, power, ground, SDA=21, SCL=22, and connector continuity |
| Full white pattern fails | Run the isolated diagnostic; investigate controller/module compatibility before gameplay |
| Border shifted or clipped | Record which edges fail; investigate controller offsets and panel identity |
| Image rotated | Confirm diagnostic text/corners before changing remap or COM settings |
| Corruption after gameplay starts | Record diagnostic results and serial log; framebuffer size and timing must both be considered |

## Optional hardware roadmap

The source has optional button mappings on GPIO32, 33, 25, 26, and 27, using internal pull-ups and active-low contacts to GND. None is confirmed installed. A future input guide should include the actual switch wiring, pin confirmation, debounce test, and gameplay validation.

A future buzzer, battery, joystick, or enclosure requires a separate electrical design and test plan. These components are not part of the current baseline.
