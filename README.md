# Smart BMW Watch ⌚🚗

ESP32-S3 round-display dashboard firmware for a BMW-style watch UI.

This project runs on a Waveshare ESP32-S3 1.85" round LCD board (360x360) using LVGL 9.x. It includes multi-screen telemetry UI, IMU + RTC support, optional SD/I2S audio, and a pluggable OBD2 provider layer (currently mock data by default).

## Highlights ✨

- 360x360 round UI with ST77916 QSPI display driver
- LVGL 9.x screen system with animated transitions
- 18 screens (watchface, speed, RPM, engine, temperatures, turbo, driving, electrical, body, performance, DTC, settings, etc.)
- Button navigation on GPIO0 (short press = next screen)
- QMI8658 IMU integration (G-force style data)
- PCF85063 RTC integration
- Background NTP sync task (WiFi on-demand, then off)
- OBD2 provider abstraction for easy swap from mock to real BLE/ELM327 provider
- Optional SD card + I2S audio playback path

## Project Layout 📁

- `src/main.cpp`: board bring-up, display init, LVGL loop, module init
- `src/screen_manager.cpp`: screen registry, transitions, button handling
- `src/screen_*.cpp`: individual LVGL screens
- `src/obd2_provider.cpp`: mock telemetry provider implementation
- `src/imu.cpp`: QMI8658C driver (I2C)
- `src/rtc.cpp`: PCF85063 RTC driver (I2C)
- `src/audio.cpp`: SD_MMC + I2S audio playback
- `src/esp_lcd_st77916.*`: panel driver + init sequences
- `include/*.h`: module interfaces and screen declarations

## Hardware Notes 🔌

### Board and Display 🖥️

- Board target: `esp32-s3-devkitc1-n16r8`
- Display: ST77916, QSPI, 360x360
- Backlight: GPIO5 (PWM)

Display pins used by firmware:

- CS: GPIO21
- CLK: GPIO40
- D0: GPIO46
- D1: GPIO45
- D2: GPIO42
- D3: GPIO41
- RST: GPIO3
- BL: GPIO5

### Input 🎛️

- Button: GPIO0 (BOOT/side button)

### I2C Peripherals 🧭

- I2C bus: SDA=11, SCL=10, 400kHz
- IMU: QMI8658C at 0x6A/0x6B (auto-detected)
- RTC: PCF85063 at 0x51

### Audio (optional) 🔊

I2S DAC output (PCM5101 style):

- BCLK: GPIO48
- WS: GPIO38
- DOUT: GPIO47

SD_MMC (1-bit mode):

- CLK: GPIO14
- CMD: GPIO17
- D0: GPIO16

## Software Architecture 🧠

### Main loop 🔁

`setup()` initializes:

1. LCD + panel init sequence detection
2. LVGL display and buffers
3. Button handling
4. OBD2 provider (mock by default)
5. IMU
6. RTC
7. NTP sync task
8. Screen manager

`loop()` updates:

- button
- OBD2 provider
- IMU
- RTC
- active screen
- audio loop
- LVGL timer handler

### Screen system 🪟

Screens implement a simple interface:

- `create(parent)`
- `update()`
- `destroy()`

The manager keeps a screen registry and performs animated transitions when switching screens.

### OBD2 provider abstraction 📡

Provider API is function-pointer based:

- `init()`
- `update()`
- `get_data()`

Current default provider: `obd2_mock_provider`.

To add real BMW data, implement another provider (for example BLE ELM327) and inject it with `obd2_set_provider(...)`.

## Build and Upload 🛠️

### Prerequisites ✅

- VS Code + PlatformIO extension
- USB connection to board
- Correct COM port

### PlatformIO config ⚙️

Environment: `[env:esp32-s3-devkitc1-n16r8]`

Key config values:

- framework: Arduino
- monitor speed: 115200
- partitions: `huge_app.csv`
- extra script: `extra_script.py`

### Build 🧱

From this project folder:

```bash
platformio run
```

### Upload ⬆️

```bash
platformio run -t upload --upload-port COM6
```

### Monitor 🖨️

```bash
platformio device monitor -b 115200
```

## Important Runtime Config 🧩

### WiFi credentials for NTP 📶

NTP sync currently uses hardcoded credentials in `src/main.cpp`:

- SSID: `WS168`
- Password: `12345678`

Change these values before production use.

### Audio startup 🎵

Audio init/play is currently disabled in `setup()` comments. Uncomment the audio calls in `src/main.cpp` if you want boot-time playback.

## Why extra_script.py exists 🧹

`extra_script.py` removes ARM-specific LVGL assembly files (`helium`/`neon`) from `.pio/libdeps/.../lvgl` before build. This avoids non-ESP32 assembly sources from being compiled accidentally.

## Troubleshooting 🩺

- 🖤 Black screen or garbage lines:
  - Check panel wiring and power.
  - Confirm ST77916 init logs over serial.
  - Ensure backlight GPIO5 is active.
- 🧭 IMU not found:
  - Verify I2C wiring and bus pull-ups.
  - Check addresses 0x6A/0x6B.
- 🕒 RTC not found:
  - Confirm device at 0x51 on same I2C bus.
- 🔌 Upload failure:
  - Verify correct COM port and cable.
  - Hold BOOT button if needed during flashing.
- 🔊 No audio:
  - Confirm SD card mount and file path.
  - Verify I2S DAC wiring.

## Next Steps 🚀

- Replace mock OBD2 provider with BLE ELM327 or CAN-backed provider.
- Move WiFi credentials to a secure/local config mechanism.
- Add power management and long-press actions (sleep/off).
- Add tests for provider math and screen update formatting.
