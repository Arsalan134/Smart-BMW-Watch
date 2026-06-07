#include "obd2_provider.h"
#include <Arduino.h>
#include <math.h>
#include <string.h>

// --- Mock data state ---
static OBD2Data mock_data = {};
static unsigned long mock_start_ms = 0;

// Simulates a drive: idle -> accelerate -> cruise -> decelerate -> idle
static void mock_init(void) {
  mock_start_ms = millis();
  memset(&mock_data, 0, sizeof(mock_data));
  mock_data.connected = true;
  mock_data.rpm = 800;
  mock_data.coolant_temp = 72;
  mock_data.voltage = 14.2f;
  mock_data.intake_temp = 35;
  mock_data.oil_temp = 65;
  mock_data.fuel_level = 72;
  mock_data.afr = 14.7f;
  mock_data.lambda = 1.0f;
  mock_data.fuel_trim_short = 0.0f;
  mock_data.fuel_trim_long = 1.2f;
  mock_data.manifold_pressure = 101.0f;
  mock_data.seatbelt_driver = true;
  mock_data.headlights_on = true;
  mock_data.parking_dist_cm = 255;
  mock_data.dtc_count = 2;
  mock_data.dtc_codes[0] = 0x0171;  // P0171 - System too lean
  mock_data.dtc_codes[1] = 0x0420;  // P0420 - Catalyst efficiency
  Serial.println("OBD2 Mock: initialized (full telemetry)");
}

static void mock_update(void) {
  // Time-based simulation cycling every 20 seconds
  float t = (float)((millis() - mock_start_ms) % 20000) / 20000.0f;
  float uptime_sec = (float)(millis() - mock_start_ms) / 1000.0f;

  // Phase 0.0-0.3: accelerate, 0.3-0.6: cruise, 0.6-0.85: decelerate, 0.85-1.0: idle
  if (t < 0.3f) {
    float p = t / 0.3f;
    mock_data.speed_kmh = (uint16_t)(p * 120);
    mock_data.rpm = 800 + (uint16_t)(p * 5200);
    mock_data.throttle_pct = 60 + (uint8_t)(p * 35);
    mock_data.accel_pedal = 70 + (uint8_t)(p * 25);
    mock_data.boost_bar = p * 1.4f;
    mock_data.boost_target = p * 1.5f;
    mock_data.fuel_rate_lph = 2.0f + p * 18.0f;
    mock_data.engine_load = 50 + (uint8_t)(p * 45);
    mock_data.gear = (p < 0.15f) ? 1 : (p < 0.4f) ? 2 : (p < 0.7f) ? 3 : 4;
    mock_data.brake_pressure = 0;
    mock_data.g_force_lon = p * 0.6f;
    mock_data.g_force_lat = sinf(t * 20.0f) * 0.2f;
    mock_data.torque_nm = 80 + (uint16_t)(p * 280);
    mock_data.hp_estimate = 30 + (uint16_t)(p * 180);
    mock_data.steering_angle = (int16_t)(sinf(t * 15.0f) * 30);
    mock_data.dsc_active = false;
    mock_data.drive_mode = 1;  // Sport during accel
  } else if (t < 0.6f) {
    float wobble = sinf((t - 0.3f) * 40.0f) * 0.05f;
    mock_data.speed_kmh = 115 + (int16_t)(wobble * 60);
    mock_data.rpm = 2800 + (int16_t)(wobble * 400);
    mock_data.throttle_pct = 30 + (uint8_t)(wobble * 50);
    mock_data.accel_pedal = 25 + (uint8_t)(wobble * 40);
    mock_data.boost_bar = 0.4f + wobble * 2.0f;
    mock_data.boost_target = 0.5f;
    mock_data.fuel_rate_lph = 8.0f + wobble * 4.0f;
    mock_data.engine_load = 40 + (uint8_t)(wobble * 200);
    mock_data.gear = 5;
    mock_data.brake_pressure = 0;
    mock_data.g_force_lon = wobble * 0.1f;
    mock_data.g_force_lat = sinf((t - 0.3f) * 30.0f) * 0.4f;
    mock_data.torque_nm = 150 + (int16_t)(wobble * 500);
    mock_data.hp_estimate = 80 + (uint16_t)(wobble * 400);
    mock_data.steering_angle = (int16_t)(sinf((t - 0.3f) * 8.0f) * 60);
    mock_data.dsc_active = (fabsf(mock_data.g_force_lat) > 0.3f);
    mock_data.drive_mode = 0;  // Comfort during cruise
  } else if (t < 0.85f) {
    float p = (t - 0.6f) / 0.25f;
    mock_data.speed_kmh = (uint16_t)(120 * (1.0f - p));
    mock_data.rpm = 6000 - (uint16_t)(p * 5200);
    mock_data.throttle_pct = (uint8_t)((1.0f - p) * 20);
    mock_data.accel_pedal = 0;
    mock_data.boost_bar = (1.0f - p) * 0.3f;
    mock_data.boost_target = 0.0f;
    mock_data.fuel_rate_lph = 1.0f;
    mock_data.engine_load = (uint8_t)((1.0f - p) * 30);
    mock_data.gear = (p > 0.7f) ? 2 : (p > 0.4f) ? 3 : 4;
    mock_data.brake_pressure = (uint8_t)(p * 60);
    mock_data.g_force_lon = -p * 0.8f;
    mock_data.g_force_lat = sinf(t * 10.0f) * 0.1f;
    mock_data.torque_nm = (uint16_t)((1.0f - p) * 100);
    mock_data.hp_estimate = (uint16_t)((1.0f - p) * 60);
    mock_data.steering_angle = (int16_t)(sinf(t * 5.0f) * 15);
    mock_data.dsc_active = (p > 0.5f && mock_data.speed_kmh > 40);
    mock_data.drive_mode = 0;
  } else {
    float idle_t = (t - 0.85f) / 0.15f;
    mock_data.speed_kmh = 0;
    mock_data.rpm = 780 + (uint16_t)(sinf(t * 100.0f) * 30);
    mock_data.throttle_pct = 0;
    mock_data.accel_pedal = 0;
    mock_data.boost_bar = 0.0f;
    mock_data.boost_target = 0.0f;
    mock_data.fuel_rate_lph = 0.8f;
    mock_data.engine_load = 15;
    mock_data.gear = 0;  // Neutral
    mock_data.brake_pressure = (uint8_t)(30 * (1.0f - idle_t));
    mock_data.g_force_lon = 0.0f;
    mock_data.g_force_lat = 0.0f;
    mock_data.torque_nm = 20;
    mock_data.hp_estimate = 0;
    mock_data.steering_angle = 0;
    mock_data.dsc_active = false;
    mock_data.drive_mode = 2;  // Eco at idle
  }

  // Temps (slowly evolve)
  mock_data.coolant_temp = 70 + (int16_t)(20.0f * (1.0f - expf(-uptime_sec / 30.0f)));
  mock_data.oil_temp = 60 + (int16_t)(35.0f * (1.0f - expf(-uptime_sec / 45.0f)));
  mock_data.intake_temp = 30 + (int16_t)(mock_data.boost_bar * 15.0f);

  // Electrical
  mock_data.voltage = 14.2f - (mock_data.throttle_pct / 100.0f) * 0.6f;

  // Fuel system
  mock_data.afr = 14.7f - (mock_data.throttle_pct > 80 ? 1.5f : 0.0f);
  mock_data.lambda = mock_data.afr / 14.7f;
  mock_data.fuel_trim_short = sinf(uptime_sec * 0.5f) * 3.0f;
  mock_data.fuel_trim_long = 1.2f + sinf(uptime_sec * 0.1f) * 0.5f;
  mock_data.manifold_pressure = 30.0f + mock_data.boost_bar * 70.0f + mock_data.throttle_pct * 0.7f;
  mock_data.fuel_level = 72;  // Static

  // Ignition
  mock_data.ignition_advance = 12.0f + (mock_data.rpm / 1000.0f) * 3.0f - mock_data.boost_bar * 4.0f;
  mock_data.misfire_count = (uptime_sec > 60.0f) ? 2 : 0;

  // Wheel speeds (slight variation simulates slip)
  uint16_t base_ws = mock_data.speed_kmh * 10;
  mock_data.wheel_speed_fl = base_ws + (uint16_t)(sinf(t * 50.0f) * 5);
  mock_data.wheel_speed_fr = base_ws + (uint16_t)(sinf(t * 53.0f) * 5);
  mock_data.wheel_speed_rl = base_ws + (uint16_t)(sinf(t * 47.0f) * 8);
  mock_data.wheel_speed_rr = base_ws + (uint16_t)(sinf(t * 51.0f) * 8);

  // Drift angle (based on lateral G and steering)
  mock_data.drift_angle = mock_data.g_force_lat * 12.0f;

  // 0-100 timer (fake static value)
  mock_data.zero_to_100_sec = 6.8f;

  // Body (static for mock)
  mock_data.doors_open = 0;  // All closed
  mock_data.turn_left = (((int)(uptime_sec * 2)) % 4 == 0);
  mock_data.turn_right = false;
}

static const OBD2Data* mock_get_data(void) {
  return &mock_data;
}

// --- Exported provider ---
const OBD2Provider obd2_mock_provider = {
    mock_init,
    mock_update,
    mock_get_data,
    "Mock OBD2",
};

// --- Global provider management ---
static const OBD2Provider* active_provider = nullptr;

void obd2_set_provider(const OBD2Provider* provider) {
  active_provider = provider;
}

const OBD2Data* obd2_get_data(void) {
  if (active_provider && active_provider->get_data) {
    return active_provider->get_data();
  }
  static const OBD2Data empty = {};
  return &empty;
}

void obd2_init(void) {
  if (active_provider && active_provider->init) {
    active_provider->init();
    Serial.printf("OBD2 Provider: %s\n", active_provider->name);
  }
}

void obd2_update(void) {
  if (active_provider && active_provider->update) {
    active_provider->update();
  }
}
