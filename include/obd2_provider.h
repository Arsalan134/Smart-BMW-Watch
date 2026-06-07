#pragma once
#include <stdint.h>

// --- OBD2 data structure ---
struct OBD2Data {
  bool connected;

  // Core engine
  uint16_t speed_kmh;       // 0-300
  uint16_t rpm;             // 0-8000
  int16_t coolant_temp;     // -40 to 215 °C
  int16_t intake_temp;      // -40 to 215 °C
  uint8_t throttle_pct;     // 0-100%
  uint8_t engine_load;      // 0-100%
  float boost_bar;          // 0.0 - 3.0 bar
  float manifold_pressure;  // kPa
  float boost_target;       // bar (turbo target)

  // Fuel
  float fuel_rate_lph;      // L/h
  uint8_t fuel_level;       // 0-100%
  float fuel_trim_short;    // -25 to +25%
  float fuel_trim_long;     // -25 to +25%
  float afr;                // Air/fuel ratio (14.7 = stoich)
  float lambda;             // Lambda value

  // Electrical
  float voltage;            // 0.0 - 16.0 V

  // Temperatures
  int16_t oil_temp;         // -40 to 200 °C

  // Driving
  int16_t steering_angle;   // -540 to +540 degrees
  uint8_t gear;             // 0=N, 1-6, 7=R
  uint8_t brake_pressure;   // 0-100%
  uint8_t accel_pedal;      // 0-100%
  uint8_t drive_mode;       // 0=Comfort, 1=Sport, 2=Eco
  bool dsc_active;          // DSC intervening
  uint16_t wheel_speed_fl;  // km/h * 10
  uint16_t wheel_speed_fr;
  uint16_t wheel_speed_rl;
  uint16_t wheel_speed_rr;

  // Engine detail
  uint16_t torque_nm;       // Nm
  float ignition_advance;   // degrees
  uint16_t misfire_count;   // total misfires
  uint16_t hp_estimate;     // Estimated HP

  // Body
  uint8_t doors_open;       // Bitmask: bit0=FL, bit1=FR, bit2=RL, bit3=RR, bit4=trunk
  bool seatbelt_driver;
  bool headlights_on;
  bool turn_left;
  bool turn_right;
  uint8_t parking_dist_cm;  // Closest obstacle cm (0=none, 255=clear)

  // Performance
  float g_force_lat;        // lateral G (-2.0 to +2.0)
  float g_force_lon;        // longitudinal G (-2.0 to +2.0)
  float zero_to_100_sec;    // Last 0-100 time (0 = not measured)
  float drift_angle;        // degrees (0 = straight)

  // DTCs
  uint8_t dtc_count;        // Number of active trouble codes
  uint32_t dtc_codes[8];    // Up to 8 DTC codes
};

// --- OBD2 Provider interface (function pointers for DI) ---
typedef struct {
  void (*init)(void);
  void (*update)(void);
  const OBD2Data* (*get_data)(void);
  const char* name;
} OBD2Provider;

// --- Available providers ---
extern const OBD2Provider obd2_mock_provider;
// extern const OBD2Provider obd2_ble_provider;  // Future: real ELM327 BLE

// --- Global accessor ---
void obd2_set_provider(const OBD2Provider* provider);
const OBD2Data* obd2_get_data(void);
void obd2_init(void);
void obd2_update(void);
