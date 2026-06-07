#include "imu.h"
#include <Arduino.h>
#include <Wire.h>
#include <math.h>

// QMI8658C I2C config for Waveshare ESP32-S3-Touch-LCD-1.85
// Confirmed from schematic: SDA=11, SCL=10
#define IMU_SDA 11
#define IMU_SCL 10

// QMI8658 possible I2C addresses (SA0 pin determines)
#define QMI8658_ADDR_HIGH 0x6B  // SA0 = 1
#define QMI8658_ADDR_LOW 0x6A   // SA0 = 0

// QMI8658 registers
#define QMI8658_WHO_AM_I 0x00
#define QMI8658_CTRL1 0x02
#define QMI8658_CTRL2 0x03  // Accelerometer config
#define QMI8658_CTRL3 0x04  // Gyroscope config
#define QMI8658_CTRL5 0x06  // Sensor data processing
#define QMI8658_CTRL7 0x08  // Enable sensors
#define QMI8658_AX_L 0x35   // Accel X low byte
#define QMI8658_RESET 0x60

static IMUData imu_data = {};
static bool imu_ready = false;
static uint8_t qmi_addr = 0;  // Detected address

static bool qmi_write_reg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(qmi_addr);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

static uint8_t qmi_read_reg(uint8_t reg) {
  Wire.beginTransmission(qmi_addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(qmi_addr, (uint8_t)1);
  return Wire.available() ? Wire.read() : 0;
}

static bool qmi_read_accel(int16_t* ax, int16_t* ay, int16_t* az) {
  Wire.beginTransmission(qmi_addr);
  Wire.write(QMI8658_AX_L);
  if (Wire.endTransmission(false) != 0)
    return false;

  uint8_t got = Wire.requestFrom(qmi_addr, (uint8_t)6);
  if (got < 6)
    return false;

  uint8_t buf[6];
  for (int i = 0; i < 6; i++) {
    buf[i] = Wire.read();
  }

  *ax = (int16_t)(buf[1] << 8 | buf[0]);
  *ay = (int16_t)(buf[3] << 8 | buf[2]);
  *az = (int16_t)(buf[5] << 8 | buf[4]);
  return true;
}

// Try to find QMI8658 at given address
static bool qmi_probe(uint8_t addr) {
  Wire.beginTransmission(addr);
  if (Wire.endTransmission() != 0)
    return false;

  // Read WHO_AM_I
  Wire.beginTransmission(addr);
  Wire.write(QMI8658_WHO_AM_I);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)1);
  uint8_t id = Wire.available() ? Wire.read() : 0;

  Serial.printf("IMU: Probe 0x%02X -> WHO_AM_I=0x%02X\n", addr, id);
  return (id == 0x05 || id == 0x06);
}

void imu_init(void) {
  Serial.println("IMU: Starting init...");
  Wire.begin(IMU_SDA, IMU_SCL, 400000);
  delay(50);

  Serial.printf("IMU: I2C on SDA=%d SCL=%d\n", IMU_SDA, IMU_SCL);

  // Auto-detect QMI8658 address
  if (qmi_probe(QMI8658_ADDR_HIGH)) {
    qmi_addr = QMI8658_ADDR_HIGH;
  } else if (qmi_probe(QMI8658_ADDR_LOW)) {
    qmi_addr = QMI8658_ADDR_LOW;
  } else {
    Serial.println("IMU: QMI8658C not found at 0x6A or 0x6B!");
    imu_ready = false;
    return;
  }
  Serial.printf("IMU: QMI8658C detected at 0x%02X\n", qmi_addr);

  // Soft reset
  qmi_write_reg(QMI8658_RESET, 0xB0);
  delay(50);

  // Verify device is back after reset
  uint8_t id = qmi_read_reg(QMI8658_WHO_AM_I);
  if (id != 0x05 && id != 0x06) {
    Serial.printf("IMU: Failed after reset (ID=0x%02X)\n", id);
    imu_ready = false;
    return;
  }

  // CTRL1: Address auto-increment enabled
  qmi_write_reg(QMI8658_CTRL1, 0x40);
  delay(2);

  // CTRL2: Accel ±8G range, 500Hz ODR
  // Bits[6:4] = 010 (±8G), Bits[3:0] = 0101 (500Hz)
  qmi_write_reg(QMI8658_CTRL2, 0x25);
  delay(2);

  // CTRL3: Gyro ±2048dps, 500Hz
  qmi_write_reg(QMI8658_CTRL3, 0x65);
  delay(2);

  // CTRL5: No low-pass filter (default)
  qmi_write_reg(QMI8658_CTRL5, 0x00);
  delay(2);

  // CTRL7: Enable accelerometer and gyroscope
  qmi_write_reg(QMI8658_CTRL7, 0x03);
  delay(50);  // Wait for sensors to start producing data

  // Verify CTRL7 was written
  uint8_t ctrl7 = qmi_read_reg(QMI8658_CTRL7);
  Serial.printf("IMU: CTRL7 readback = 0x%02X (expect 0x03)\n", ctrl7);

  // Do a test read
  int16_t ax, ay, az;
  if (qmi_read_accel(&ax, &ay, &az)) {
    Serial.printf("IMU: Test read OK: ax=%d ay=%d az=%d\n", ax, ay, az);
  } else {
    Serial.println("IMU: Test read FAILED");
  }

  imu_ready = true;
  Serial.println("IMU: QMI8658C initialized (±8G, 500Hz)");
}

void imu_update(void) {
  if (!imu_ready) {
    // Retry init every 5 seconds so user can see bus scan in monitor
    static unsigned long last_retry = 0;
    if (millis() - last_retry > 5000) {
      last_retry = millis();
      Serial.println("IMU: Retrying init...");
      imu_init();
    }
    return;
  }

  int16_t ax, ay, az;
  if (!qmi_read_accel(&ax, &ay, &az))
    return;

  // Convert to G (±8G range: sensitivity = 4096 LSB/G)
  const float scale = 8.0f / 32768.0f;
  imu_data.accel_x = ax * scale;
  imu_data.accel_y = ay * scale;
  imu_data.accel_z = az * scale;

  // Total lateral G (X and Y axes, ignoring gravity on Z)
  float gx = imu_data.accel_x;
  float gy = imu_data.accel_y;
  imu_data.g_total = sqrtf(gx * gx + gy * gy);
}

const IMUData* imu_get_data(void) {
  return &imu_data;
}
