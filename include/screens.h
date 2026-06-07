#pragma once
#include "lvgl.h"

// --- Practice Screen (empty canvas for LVGL experiments) ---
void screen_practice_create(lv_obj_t* parent);
void screen_practice_update(void);
void screen_practice_destroy(void);

// --- Empty Screen (blank page after Practice) ---
void screen_empty_create(lv_obj_t* parent);
void screen_empty_update(void);
void screen_empty_destroy(void);

// --- Watch Face Screen ---
void screen_watchface_create(lv_obj_t* parent);
void screen_watchface_update(void);
void screen_watchface_destroy(void);

// --- BMW Telemetry Screen ---
void screen_bmw_create(lv_obj_t* parent);
void screen_bmw_update(void);
void screen_bmw_destroy(void);

// --- Minimal Speed Screen ---
void screen_speed_create(lv_obj_t* parent);
void screen_speed_update(void);
void screen_speed_destroy(void);

// --- Minimal RPM + Gear Screen ---
void screen_rpm_create(lv_obj_t* parent);
void screen_rpm_update(void);
void screen_rpm_destroy(void);

// --- Engine Screen (RPM, Coolant, Throttle) ---
void screen_engine_create(lv_obj_t* parent);
void screen_engine_update(void);
void screen_engine_destroy(void);

// --- Engine Detail (Load, Trims, Lambda, Ignition) ---
void screen_engine_detail_create(lv_obj_t* parent);
void screen_engine_detail_update(void);
void screen_engine_detail_destroy(void);

// --- Temperatures (Oil, Coolant, Intake) ---
void screen_temps_create(lv_obj_t* parent);
void screen_temps_update(void);
void screen_temps_destroy(void);

// --- Pressure Screen (Boost, Intake, Fuel) ---
void screen_pressure_create(lv_obj_t* parent);
void screen_pressure_update(void);
void screen_pressure_destroy(void);

// --- Turbo Detail (Boost arc, Target, MAP) ---
void screen_turbo_detail_create(lv_obj_t* parent);
void screen_turbo_detail_update(void);
void screen_turbo_detail_destroy(void);

// --- Driving (Gear, Steering, Pedals, Mode) ---
void screen_driving_create(lv_obj_t* parent);
void screen_driving_update(void);
void screen_driving_destroy(void);

// --- Electrical Screen (Voltage, Fuel Rate) ---
void screen_electrical_create(lv_obj_t* parent);
void screen_electrical_update(void);
void screen_electrical_destroy(void);

// --- Body Status (Doors, Lights, Seatbelt) ---
void screen_body_create(lv_obj_t* parent);
void screen_body_update(void);
void screen_body_destroy(void);

// --- Performance (G-force, HP, 0-100) ---
void screen_performance_create(lv_obj_t* parent);
void screen_performance_update(void);
void screen_performance_destroy(void);

// --- DTC (Trouble Codes) ---
void screen_dtc_create(lv_obj_t* parent);
void screen_dtc_update(void);
void screen_dtc_destroy(void);

// --- BMW Stats/Info Screen ---
void screen_stats_create(lv_obj_t* parent);
void screen_stats_update(void);
void screen_stats_destroy(void);

// --- Settings Screen ---
void screen_settings_create(lv_obj_t* parent);
void screen_settings_update(void);
void screen_settings_destroy(void);
