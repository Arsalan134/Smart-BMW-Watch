#include "rtc.h"
#include <Arduino.h>
#include <Wire.h>

// PCF85063 I2C address
#define PCF85063_ADDR 0x51

// PCF85063 registers
#define PCF85063_CTRL1 0x00
#define PCF85063_CTRL2 0x01
#define PCF85063_SECONDS 0x04
#define PCF85063_MINUTES 0x05
#define PCF85063_HOURS 0x06
#define PCF85063_DAYS 0x07
#define PCF85063_WEEKDAYS 0x08
#define PCF85063_MONTHS 0x09
#define PCF85063_YEARS 0x0A

static RTCTime rtc_time = {};
static bool rtc_ready = false;
static unsigned long last_read_ms = 0;

// BCD helpers
static uint8_t bcd_to_dec(uint8_t bcd) {
  return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static uint8_t dec_to_bcd(uint8_t dec) {
  return ((dec / 10) << 4) | (dec % 10);
}

static bool pcf_write_reg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(PCF85063_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static uint8_t pcf_read_reg(uint8_t reg) {
  Wire.beginTransmission(PCF85063_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)PCF85063_ADDR, (uint8_t)1);
  return Wire.available() ? Wire.read() : 0;
}

void rtc_init(void) {
  // Wire already initialized by imu_init() on same bus (SDA=11, SCL=10)
  delay(5);

  // Check if RTC responds
  Wire.beginTransmission(PCF85063_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.println("RTC: PCF85063 not found at 0x51");
    rtc_ready = false;
    return;
  }

  // CTRL1: Normal mode, no correction, 24h mode, RTC runs
  // Bit 5 (STOP) = 0 to ensure clock is running
  uint8_t ctrl1 = pcf_read_reg(PCF85063_CTRL1);
  if (ctrl1 & 0x20) {
    // Clock was stopped, start it
    pcf_write_reg(PCF85063_CTRL1, ctrl1 & ~0x20);
    Serial.println("RTC: Clock was stopped, now running");
  }

  rtc_ready = true;
  Serial.println("RTC: PCF85063 initialized");

  // Read initial time
  rtc_update();
  Serial.printf("RTC: %02d:%02d:%02d  %02d/%02d/20%02d\n",
                rtc_time.hour, rtc_time.minute, rtc_time.second,
                rtc_time.day, rtc_time.month, rtc_time.year);
}

void rtc_update(void) {
  if (!rtc_ready)
    return;

  // Only read once per second (RTC resolution is 1s)
  unsigned long now = millis();
  if (now - last_read_ms < 1000)
    return;
  last_read_ms = now;

  // Burst read 7 time registers starting at SECONDS (0x04)
  Wire.beginTransmission(PCF85063_ADDR);
  Wire.write(PCF85063_SECONDS);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)PCF85063_ADDR, (uint8_t)7);

  if (Wire.available() < 7)
    return;

  uint8_t sec = Wire.read();
  uint8_t min = Wire.read();
  uint8_t hr = Wire.read();
  uint8_t day = Wire.read();
  uint8_t wd = Wire.read();
  uint8_t mon = Wire.read();
  uint8_t yr = Wire.read();

  // Check oscillator integrity (bit 7 of seconds = OS flag)
  if (sec & 0x80) {
    // Oscillator integrity not guaranteed — time may be invalid
    // Clear the flag by writing back
    pcf_write_reg(PCF85063_SECONDS, sec & 0x7F);
  }

  rtc_time.second = bcd_to_dec(sec & 0x7F);
  rtc_time.minute = bcd_to_dec(min & 0x7F);
  rtc_time.hour = bcd_to_dec(hr & 0x3F);
  rtc_time.day = bcd_to_dec(day & 0x3F);
  rtc_time.weekday = wd & 0x07;
  rtc_time.month = bcd_to_dec(mon & 0x1F);
  rtc_time.year = bcd_to_dec(yr);
}

const RTCTime* rtc_get_time(void) {
  return &rtc_time;
}

void rtc_set_time(const RTCTime* t) {
  if (!rtc_ready || !t)
    return;

  // Stop clock during write for consistency
  uint8_t ctrl1 = pcf_read_reg(PCF85063_CTRL1);
  pcf_write_reg(PCF85063_CTRL1, ctrl1 | 0x20);  // STOP bit

  pcf_write_reg(PCF85063_SECONDS, dec_to_bcd(t->second));
  pcf_write_reg(PCF85063_MINUTES, dec_to_bcd(t->minute));
  pcf_write_reg(PCF85063_HOURS, dec_to_bcd(t->hour));
  pcf_write_reg(PCF85063_DAYS, dec_to_bcd(t->day));
  pcf_write_reg(PCF85063_WEEKDAYS, t->weekday & 0x07);
  pcf_write_reg(PCF85063_MONTHS, dec_to_bcd(t->month));
  pcf_write_reg(PCF85063_YEARS, dec_to_bcd(t->year));

  // Restart clock
  pcf_write_reg(PCF85063_CTRL1, ctrl1 & ~0x20);

  // Update local copy
  rtc_time = *t;
  Serial.printf("RTC: Time set to %02d:%02d:%02d  %02d/%02d/20%02d\n",
                t->hour, t->minute, t->second, t->day, t->month, t->year);
}
