#pragma once
#include <stdint.h>

// QMI8658 IMU on Waveshare ESP32-S3-Touch-LCD-1.85B
// I2C address 0x6B, SDA=6, SCL=7

struct IMUData {
  float accel_x;  // G (-16 to +16)
  float accel_y;  // G
  float accel_z;  // G
  float g_total;  // Total G magnitude (excluding gravity)
};

void imu_init(void);
void imu_update(void);
const IMUData* imu_get_data(void);
