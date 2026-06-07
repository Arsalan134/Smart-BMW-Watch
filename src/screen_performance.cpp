#include <Arduino.h>
#include <math.h>
#include "design.h"
#include "imu.h"
#include "obd2_provider.h"
#include "screens.h"

// Performance: G-force meter using onboard IMU, current G, max G (5s reset),
// HP, torque, 0-100 timer (auto-reset when stopped)
static lv_obj_t* g_dot = NULL;
static lv_obj_t* g_ring = NULL;
static lv_obj_t* lbl_g_cur = NULL;
static lv_obj_t* lbl_g_max = NULL;
static lv_obj_t* lbl_hp = NULL;
static lv_obj_t* lbl_0100 = NULL;
static lv_obj_t* lbl_torque = NULL;

// Max G tracking
static float max_g = 0.0f;
static unsigned long max_g_time = 0;

// 0-100 timer state
static bool timing_active = false;
static unsigned long timing_start = 0;
static float last_0100_result = 0.0f;
static bool was_stopped = false;

// G-force meter center and radius
#define GF_CX 0
#define GF_CY -35
#define GF_RADIUS 60

void screen_performance_create(lv_obj_t* parent) {
  // G-force ring (circle background)
  g_ring = lv_arc_create(parent);
  lv_obj_set_size(g_ring, GF_RADIUS * 2 + 10, GF_RADIUS * 2 + 10);
  lv_arc_set_rotation(g_ring, 0);
  lv_arc_set_bg_angles(g_ring, 0, 360);
  lv_arc_set_range(g_ring, 0, 100);
  lv_arc_set_value(g_ring, 0);
  lv_obj_set_style_arc_width(g_ring, 2, LV_PART_MAIN);
  lv_obj_set_style_arc_color(g_ring, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_arc_width(g_ring, 0, LV_PART_INDICATOR);
  lv_obj_remove_style(g_ring, NULL, LV_PART_KNOB);
  lv_obj_remove_flag(g_ring, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(g_ring, LV_ALIGN_CENTER, GF_CX, GF_CY);

  // Crosshair lines (horizontal)
  lv_obj_t* h_line = lv_obj_create(parent);
  lv_obj_set_size(h_line, GF_RADIUS * 2 - 10, 1);
  lv_obj_set_style_bg_color(h_line, COLOR_ARC_BG, 0);
  lv_obj_set_style_bg_opa(h_line, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(h_line, 0, 0);
  lv_obj_align(h_line, LV_ALIGN_CENTER, GF_CX, GF_CY);

  // Crosshair vertical
  lv_obj_t* v_line = lv_obj_create(parent);
  lv_obj_set_size(v_line, 1, GF_RADIUS * 2 - 10);
  lv_obj_set_style_bg_color(v_line, COLOR_ARC_BG, 0);
  lv_obj_set_style_bg_opa(v_line, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(v_line, 0, 0);
  lv_obj_align(v_line, LV_ALIGN_CENTER, GF_CX, GF_CY);

  // G-force dot
  g_dot = lv_obj_create(parent);
  lv_obj_set_size(g_dot, 12, 12);
  lv_obj_set_style_radius(g_dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(g_dot, COLOR_WHITE, 0);
  lv_obj_set_style_bg_opa(g_dot, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(g_dot, 0, 0);
  lv_obj_align(g_dot, LV_ALIGN_CENTER, GF_CX, GF_CY);

  // Current G and Max G labels (inside the ring area, compact)
  lv_obj_t* t_cur = lv_label_create(parent);
  lv_obj_set_style_text_font(t_cur, FONT_SM, 0);
  lv_obj_set_style_text_color(t_cur, COLOR_LABEL, 0);
  lv_label_set_text(t_cur, "G");
  lv_obj_align(t_cur, LV_ALIGN_CENTER, -45, GF_CY + GF_RADIUS + 10);

  lbl_g_cur = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_g_cur, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_g_cur, COLOR_WHITE, 0);
  lv_label_set_text(lbl_g_cur, "0.00");
  lv_obj_align(lbl_g_cur, LV_ALIGN_CENTER, -45, GF_CY + GF_RADIUS + 24);

  lv_obj_t* t_max = lv_label_create(parent);
  lv_obj_set_style_text_font(t_max, FONT_SM, 0);
  lv_obj_set_style_text_color(t_max, COLOR_LABEL, 0);
  lv_label_set_text(t_max, "MAX");
  lv_obj_align(t_max, LV_ALIGN_CENTER, 45, GF_CY + GF_RADIUS + 10);

  lbl_g_max = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_g_max, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_g_max, COLOR_WHITE, 0);
  lv_label_set_text(lbl_g_max, "0.00");
  lv_obj_align(lbl_g_max, LV_ALIGN_CENTER, 45, GF_CY + GF_RADIUS + 24);

  // HP + Torque + 0-100 bottom row
  lv_obj_t* t_hp = lv_label_create(parent);
  lv_obj_set_style_text_font(t_hp, FONT_SM, 0);
  lv_obj_set_style_text_color(t_hp, COLOR_LABEL, 0);
  lv_label_set_text(t_hp, "HP");
  lv_obj_align(t_hp, LV_ALIGN_CENTER, -60, 75);

  lbl_hp = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_hp, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_hp, COLOR_WHITE, 0);
  lv_label_set_text(lbl_hp, "0");
  lv_obj_align(lbl_hp, LV_ALIGN_CENTER, -60, 93);

  lv_obj_t* t_tq = lv_label_create(parent);
  lv_obj_set_style_text_font(t_tq, FONT_SM, 0);
  lv_obj_set_style_text_color(t_tq, COLOR_LABEL, 0);
  lv_label_set_text(t_tq, "Nm");
  lv_obj_align(t_tq, LV_ALIGN_CENTER, 0, 75);

  lbl_torque = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_torque, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_torque, COLOR_WHITE, 0);
  lv_label_set_text(lbl_torque, "0");
  lv_obj_align(lbl_torque, LV_ALIGN_CENTER, 0, 93);

  lv_obj_t* t_100 = lv_label_create(parent);
  lv_obj_set_style_text_font(t_100, FONT_SM, 0);
  lv_obj_set_style_text_color(t_100, COLOR_LABEL, 0);
  lv_label_set_text(t_100, "0-100");
  lv_obj_align(t_100, LV_ALIGN_CENTER, 60, 75);

  lbl_0100 = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_0100, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_0100, COLOR_WHITE, 0);
  lv_label_set_text(lbl_0100, "--");
  lv_obj_align(lbl_0100, LV_ALIGN_CENTER, 60, 93);

  // Reset state
  max_g = 0.0f;
  max_g_time = millis();
  timing_active = false;
  last_0100_result = 0.0f;
  was_stopped = true;
}

void screen_performance_update(void) {
  const OBD2Data* d = obd2_get_data();
  const IMUData* imu = imu_get_data();
  if (!d)
    return;
  char buf[16];
  unsigned long now = millis();

  // Use IMU for G-force (X = lateral, Y = longitudinal)
  float gx = imu->accel_x;
  float gy = imu->accel_y;
  float cur_g = imu->g_total;

  // Max G with 5-second decay
  if (cur_g > max_g) {
    max_g = cur_g;
    max_g_time = now;
  } else if (now - max_g_time > 5000) {
    max_g = cur_g;  // Reset to current
    max_g_time = now;
  }

  // 0-100 timer logic:
  // When car is stopped (speed < 3), arm the timer
  // When speed crosses 3 km/h going up, start timing
  // When speed >= 100, record result and stop
  // Timer resets when car stops again
  if (d->speed_kmh < 3) {
    if (!was_stopped) {
      was_stopped = true;
      timing_active = false;
    }
  } else if (was_stopped && d->speed_kmh >= 3) {
    // Just started moving - begin timing
    timing_active = true;
    timing_start = now;
    was_stopped = false;
    last_0100_result = 0.0f;
  }

  if (timing_active && d->speed_kmh >= 100) {
    last_0100_result = (float)(now - timing_start) / 1000.0f;
    timing_active = false;
  }

  // Move G-force dot using IMU data
  if (g_dot) {
    float px = gy * GF_RADIUS;   // IMU Y -> screen X (lateral)
    float py = -gx * GF_RADIUS;  // IMU X -> screen Y (longitudinal, inverted)
    float dist = sqrtf(px * px + py * py);
    if (dist > GF_RADIUS - 6) {
      float scale = (GF_RADIUS - 6) / dist;
      px *= scale;
      py *= scale;
    }
    lv_obj_align(g_dot, LV_ALIGN_CENTER, GF_CX + (int)px, GF_CY + (int)py);
  }

  if (lbl_g_cur) {
    snprintf(buf, sizeof(buf), "%.2f", cur_g);
    lv_label_set_text(lbl_g_cur, buf);
  }
  if (lbl_g_max) {
    snprintf(buf, sizeof(buf), "%.2f", max_g);
    lv_label_set_text(lbl_g_max, buf);
  }
  if (lbl_hp) {
    snprintf(buf, sizeof(buf), "%d", d->hp_estimate);
    lv_label_set_text(lbl_hp, buf);
  }
  if (lbl_torque) {
    snprintf(buf, sizeof(buf), "%d", d->torque_nm);
    lv_label_set_text(lbl_torque, buf);
  }
  if (lbl_0100) {
    if (timing_active) {
      float elapsed = (float)(now - timing_start) / 1000.0f;
      snprintf(buf, sizeof(buf), "%.1fs", elapsed);
    } else if (last_0100_result > 0.1f) {
      snprintf(buf, sizeof(buf), "%.1fs", last_0100_result);
    } else {
      snprintf(buf, sizeof(buf), "--");
    }
    lv_label_set_text(lbl_0100, buf);
  }
}

void screen_performance_destroy(void) {
  g_dot = NULL;
  g_ring = NULL;
  lbl_g_cur = NULL;
  lbl_g_max = NULL;
  lbl_hp = NULL;
  lbl_0100 = NULL;
  lbl_torque = NULL;
}
