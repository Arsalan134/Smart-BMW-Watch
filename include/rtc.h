#pragma once
#include <stdint.h>

// PCF85063 RTC on Waveshare ESP32-S3-Touch-LCD-1.85B
// I2C address 0x51, shared bus with QMI8658 (SDA=6, SCL=7)

struct RTCTime {
  uint8_t second;   // 0-59
  uint8_t minute;   // 0-59
  uint8_t hour;     // 0-23
  uint8_t day;      // 1-31
  uint8_t weekday;  // 0=Sun, 1=Mon, ... 6=Sat
  uint8_t month;    // 1-12
  uint8_t year;     // 0-99 (offset from 2000)
};

void rtc_init(void);
void rtc_update(void);
const RTCTime* rtc_get_time(void);
void rtc_set_time(const RTCTime* t);
