#include <Arduino.h>
#include <Audio.h>
#include <SD_MMC.h>
#include <Wire.h>
#include "audio_player.h"

// =============================================================
// PIN DEFINITIONS - ESP32-S3-Touch-LCD-1.85
// =============================================================

// I2S output to PCM5101 DAC (speaker)
#define I2S_BCLK 48
#define I2S_WS 38
#define I2S_DOUT 47

// SD Card (SDMMC 1-bit mode - no CS pin needed!)
#define SD_CLK_PIN 14
#define SD_CMD_PIN 17  // CMD line (was MOSI in SPI mode)
#define SD_D0_PIN 16   // DAT0 line (was MISO in SPI mode)

// =============================================================

static Audio i2s_audio;
static bool sd_mounted = false;

void audio_player_init(void) {
  Serial.println("Audio: === INIT START ===");

  // Use SDMMC 1-bit mode (bypasses CS issue - no CS pin needed)
  Serial.printf("Audio: SD_MMC pins (CLK=%d, CMD=%d, D0=%d)\n", SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN);
  SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN);

  if (!SD_MMC.begin("/sdcard", true)) {  // true = 1-bit mode
    Serial.println("Audio: ERROR - SD_MMC mount failed! (no card or wrong pins?)");
    goto init_i2s_only;
  }
  sd_mounted = true;

  {
    uint8_t cardType = SD_MMC.cardType();
    Serial.printf("Audio: SD card type: %d (1=MMC, 2=SD, 3=SDHC)\n", cardType);
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("Audio: SD card size: %llu MB\n", cardSize);

    // List root directory files
    File root = SD_MMC.open("/");
    if (root) {
      Serial.println("Audio: Files on SD card root:");
      File file = root.openNextFile();
      int count = 0;
      while (file && count < 20) {
        Serial.printf("  [%s] %s (%d bytes)\n",
                      file.isDirectory() ? "DIR" : "FILE",
                      file.name(), file.size());
        file = root.openNextFile();
        count++;
      }
      root.close();
      if (count == 0)
        Serial.println("  (empty - no files found!)");
    } else {
      Serial.println("Audio: ERROR - Cannot open root directory!");
    }
  }

init_i2s_only:
  // Initialize I2S audio output
  Serial.printf("Audio: I2S setPinout(BCLK=%d, WS=%d, DOUT=%d)\n", I2S_BCLK, I2S_WS, I2S_DOUT);
  i2s_audio.setPinout(I2S_BCLK, I2S_WS, I2S_DOUT);
  i2s_audio.setVolume(21);  // 0..21
  Serial.println("Audio: === INIT DONE ===");
}

void audio_player_play(const char* path) {
  Serial.printf("Audio: play requested: '%s'\n", path);
  if (!sd_mounted) {
    Serial.println("Audio: ERROR - SD not mounted, cannot play!");
    return;
  }

  // Check if file exists (try with and without leading slash)
  String pathStr(path);
  if (!pathStr.startsWith("/"))
    pathStr = "/" + pathStr;

  if (!SD_MMC.exists(pathStr.c_str())) {
    Serial.printf("Audio: ERROR - File not found: '%s'\n", pathStr.c_str());
    return;
  }

  Serial.printf("Audio: File found: '%s'\n", pathStr.c_str());
  i2s_audio.connecttoFS(SD_MMC, pathStr.c_str());
  Serial.printf("Audio: connecttoFS called, isRunning=%d\n", i2s_audio.isRunning());
}

void audio_player_stop(void) {
  i2s_audio.stopSong();
}

bool audio_player_is_playing(void) {
  return i2s_audio.isRunning();
}

void audio_player_loop(void) {
  i2s_audio.loop();
}
