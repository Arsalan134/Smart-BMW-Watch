/**
 * Smart BMW Watch - Waveshare ESP32-S3 1.85" Round LCD (360x360)
 * ST77916 QSPI driver, LVGL 9.x, Arduino framework
 * Multi-screen UI with button navigation
 */

#include <Arduino.h>
#include <WiFi.h>
#include "audio_player.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_st77916.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "imu.h"
#include "lvgl.h"
#include "obd2_provider.h"
#include "rtc.h"
#include "screen_manager.h"

// --- Pin Definitions (Waveshare ESP32-S3-Touch-LCD-1.85B) ---
#define LCD_QSPI_CS 21
#define LCD_QSPI_CLK 40
#define LCD_QSPI_D0 46
#define LCD_QSPI_D1 45
#define LCD_QSPI_D2 42
#define LCD_QSPI_D3 41
#define LCD_RST 3
#define LCD_BL 5

// --- Display Parameters ---
#define LCD_H_RES 360
#define LCD_V_RES 360
#define LCD_BIT_PER_PIXEL 16
#define LVGL_BUF_LINES 40  // Number of lines per draw buffer

// --- Globals ---
static esp_lcd_panel_handle_t panel_handle = NULL;
static lv_display_t* lvgl_display = NULL;
static uint8_t* lvgl_buf1 = NULL;
static uint8_t* lvgl_buf2 = NULL;

// --- LVGL tick callback (LVGL 9.x) ---
static uint32_t my_tick_get_cb(void) {
  return (uint32_t)millis();
}

// --- LVGL flush callback ---
static void lvgl_flush_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
  int x_start = area->x1;
  int y_start = area->y1;
  int x_end = area->x2 + 1;
  int y_end = area->y2 + 1;

  esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_end, y_end, px_map);
  lv_display_flush_ready(disp);
}

// --- Initialize QSPI bus and ST77916 panel ---
static void lcd_init(void) {
  esp_err_t ret;

  // Backlight OFF during init (use LEDC PWM like official Waveshare code)
  ledcAttach(LCD_BL, 20000, 10);  // 20kHz, 10-bit resolution
  ledcWrite(LCD_BL, 0);           // Off
  Serial.println("Backlight PWM configured (off)");

  // Hardware reset the panel first
  gpio_config_t rst_conf = {};
  rst_conf.pin_bit_mask = 1ULL << LCD_RST;
  rst_conf.mode = GPIO_MODE_OUTPUT;
  gpio_config(&rst_conf);
  gpio_set_level((gpio_num_t)LCD_RST, 0);
  vTaskDelay(pdMS_TO_TICKS(10));
  gpio_set_level((gpio_num_t)LCD_RST, 1);
  vTaskDelay(pdMS_TO_TICKS(50));
  Serial.println("Panel hardware reset done");

  // Configure SPI bus (QSPI mode)
  spi_bus_config_t buscfg = {};
  buscfg.sclk_io_num = LCD_QSPI_CLK;
  buscfg.data0_io_num = LCD_QSPI_D0;
  buscfg.data1_io_num = LCD_QSPI_D1;
  buscfg.data2_io_num = LCD_QSPI_D2;
  buscfg.data3_io_num = LCD_QSPI_D3;
  buscfg.data4_io_num = -1;
  buscfg.data5_io_num = -1;
  buscfg.data6_io_num = -1;
  buscfg.data7_io_num = -1;
  buscfg.max_transfer_sz = LCD_H_RES * LVGL_BUF_LINES * LCD_BIT_PER_PIXEL / 8;
  buscfg.flags = SPICOMMON_BUSFLAG_MASTER;

  ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
  Serial.printf("SPI bus init: %s (0x%x)\n", esp_err_to_name(ret), ret);
  if (ret != ESP_OK)
    return;

  // Step 1: Create panel IO at 5MHz (slow speed for register read)
  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_spi_config_t io_config = {};
  io_config.cs_gpio_num = LCD_QSPI_CS;
  io_config.spi_mode = 0;
  io_config.pclk_hz = 5 * 1000 * 1000;  // 5MHz for register read
  io_config.trans_queue_depth = 10;
  io_config.lcd_cmd_bits = 32;
  io_config.lcd_param_bits = 8;
  io_config.flags.quad_mode = true;

  ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle);
  Serial.printf("Panel IO create (5MHz): %s (0x%x)\n", esp_err_to_name(ret), ret);
  if (ret != ESP_OK)
    return;

  // Step 2: Read register 0x04 to determine panel version
  uint8_t register_data[4] = {0};
  int lcd_cmd = 0x04;
  lcd_cmd &= 0xff;
  lcd_cmd <<= 8;
  lcd_cmd |= (0x0B << 24);  // LCD_OPCODE_READ_CMD

  ret = esp_lcd_panel_io_rx_param(io_handle, lcd_cmd, register_data, sizeof(register_data));
  if (ret == ESP_OK) {
    Serial.printf("Register 0x04: %02x %02x %02x %02x\n",
                  register_data[0], register_data[1], register_data[2], register_data[3]);
  } else {
    Serial.printf("Register 0x04 read failed: %s - using version 1 as default\n", esp_err_to_name(ret));
  }

  // Delete slow IO handle
  esp_lcd_panel_io_del(io_handle);
  io_handle = NULL;

  // Step 3: Recreate panel IO at full speed (40MHz)
  io_config.pclk_hz = 40 * 1000 * 1000;
  ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle);
  Serial.printf("Panel IO create (40MHz): %s (0x%x)\n", esp_err_to_name(ret), ret);
  if (ret != ESP_OK)
    return;

  // Step 4: Select init sequence based on register read
  st77916_vendor_config_t vendor_config = {};
  vendor_config.flags.use_qspi_interface = 1;

  if (register_data[0] == 0x00 && register_data[1] == 0x02 &&
      register_data[2] == 0x7F && register_data[3] == 0x7F) {
    vendor_config.init_cmds = vendor_specific_init_version_2;
    vendor_config.init_cmds_size = vendor_specific_init_version_2_size;
    Serial.println("Selected init: VERSION 2");
  } else if (register_data[0] == 0x00 && register_data[1] == 0x7F &&
             register_data[2] == 0x7F && register_data[3] == 0x7F) {
    vendor_config.init_cmds = vendor_specific_init_default;
    vendor_config.init_cmds_size = vendor_specific_init_default_size;
    Serial.println("Selected init: VERSION 1");
  } else {
    // Unknown version - try version 1 (original default)
    vendor_config.init_cmds = vendor_specific_init_default;
    vendor_config.init_cmds_size = vendor_specific_init_default_size;
    Serial.println("Selected init: VERSION 1 (unknown register, using default)");
  }

  // Configure ST77916 panel
  esp_lcd_panel_dev_config_t panel_config = {};
  panel_config.reset_gpio_num = -1;  // Already reset manually above
  panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
  panel_config.bits_per_pixel = LCD_BIT_PER_PIXEL;
  panel_config.vendor_config = &vendor_config;

  ret = esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle);
  Serial.printf("Panel create: %s (0x%x)\n", esp_err_to_name(ret), ret);
  if (ret != ESP_OK)
    return;

  ret = esp_lcd_panel_init(panel_handle);
  Serial.printf("Panel init: %s (0x%x)\n", esp_err_to_name(ret), ret);
  if (ret != ESP_OK)
    return;

  ret = esp_lcd_panel_disp_on_off(panel_handle, true);
  Serial.printf("Display ON: %s (0x%x)\n", esp_err_to_name(ret), ret);

  // Clear display RAM to black (eliminates garbage/vertical lines)
  size_t clear_line_size = LCD_H_RES * LCD_BIT_PER_PIXEL / 8;
  uint8_t* clear_buf = (uint8_t*)heap_caps_calloc(1, clear_line_size, MALLOC_CAP_DMA);
  if (clear_buf) {
    for (int y = 0; y < LCD_V_RES; y++) {
      esp_lcd_panel_draw_bitmap(panel_handle, 0, y, LCD_H_RES, y + 1, clear_buf);
    }
    free(clear_buf);
    Serial.println("Display cleared to black");
  }

  // Delay before backlight
  vTaskDelay(pdMS_TO_TICKS(50));

  // Backlight ON via PWM (full brightness = 1023 for 10-bit)
  ledcWrite(LCD_BL, 1023);
  Serial.println("LCD init done, backlight ON");
}

// --- LVGL Init ---
static void lvgl_init(void) {
  lv_init();
  lv_tick_set_cb(my_tick_get_cb);

  // Create display
  lvgl_display = lv_display_create(LCD_H_RES, LCD_V_RES);
  lv_display_set_flush_cb(lvgl_display, lvgl_flush_cb);

  // Allocate draw buffers in PSRAM
  size_t buf_size = LCD_H_RES * LVGL_BUF_LINES * LCD_BIT_PER_PIXEL / 8;
  lvgl_buf1 = (uint8_t*)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
  lvgl_buf2 = (uint8_t*)heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
  lv_display_set_buffers(lvgl_display, lvgl_buf1, lvgl_buf2, buf_size,
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  Serial.println("LVGL initialized");
}

// --- Sync RTC from NTP via WiFi (runs as background task) ---
static void ntp_sync_task(void* param) {
  Serial.println("NTP: Connecting to WiFi 'WS168'...");
  WiFi.disconnect(true);
  vTaskDelay(pdMS_TO_TICKS(100));
  WiFi.mode(WIFI_STA);
  vTaskDelay(pdMS_TO_TICKS(100));
  WiFi.begin("WS168", "12345678");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("NTP: WiFi connect failed (status=%d)\n", WiFi.status());
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    vTaskDelete(NULL);
    return;
  }
  Serial.printf("NTP: WiFi connected (%s)\n", WiFi.localIP().toString().c_str());

  // Configure NTP (Berlin timezone: CET-1CEST,M3.5.0,M10.5.0/3)
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.google.com");

  // Wait for NTP sync
  struct tm timeinfo = {};
  start = millis();
  while (!getLocalTime(&timeinfo) && millis() - start < 10000) {
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  if (timeinfo.tm_year >= 120) {
    RTCTime t;
    t.second = timeinfo.tm_sec;
    t.minute = timeinfo.tm_min;
    t.hour = timeinfo.tm_hour;
    t.day = timeinfo.tm_mday;
    t.weekday = timeinfo.tm_wday;
    t.month = timeinfo.tm_mon + 1;
    t.year = timeinfo.tm_year - 100;
    rtc_set_time(&t);
    Serial.printf("NTP: RTC set to %02d:%02d:%02d  %02d/%02d/20%02d\n",
                  t.hour, t.minute, t.second, t.day, t.month, t.year);
  } else {
    Serial.println("NTP: Failed to get time");
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("NTP: Done, WiFi off");
  vTaskDelete(NULL);
}

static void start_ntp_sync(void) {
  xTaskCreatePinnedToCore(ntp_sync_task, "ntp_sync", 4096, NULL, 1, NULL, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Smart BMW Watch - Starting...");

  lcd_init();
  lvgl_init();
  button_init();

  // Inject mock OBD2 provider (swap to real BLE provider later)
  obd2_set_provider(&obd2_mock_provider);
  obd2_init();

  // Initialize onboard IMU (QMI8658)
  imu_init();

  // Initialize RTC (PCF85063 on same I2C bus)
  rtc_init();

  // Sync RTC from NTP (non-blocking background task)
  start_ntp_sync();

  screen_manager_init();

  // Render first frame immediately so display shows content
  lv_timer_handler();

  // Initialize audio (SD card + I2S DAC) - non-critical, screen already visible
  // audio_player_init();
  // Play Super Mario theme from SD card
  // audio_player_play("mario.mp3");

  Serial.println("System ready! Press button to switch screens.");
}

void loop() {
  button_update();
  obd2_update();
  imu_update();
  rtc_update();
  screen_manager_update();
  audio_player_loop();
  lv_timer_handler();
  delay(5);
}