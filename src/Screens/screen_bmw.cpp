#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// --- UI Elements ---
static lv_obj_t* lbl_speed = NULL;
static lv_obj_t* lbl_rpm = NULL;
static lv_obj_t* lbl_coolant = NULL;
static lv_obj_t* lbl_boost = NULL;
static lv_obj_t* lbl_voltage = NULL;
static lv_obj_t* lbl_status = NULL;
static lv_obj_t* arc_rpm = NULL;

void screen_bmw_create(lv_obj_t* parent) {
  // --- RPM Arc gauge ---
  arc_rpm = lv_arc_create(parent);
  lv_obj_set_size(arc_rpm, ARC_SIZE_FULL, ARC_SIZE_FULL);
  lv_obj_center(arc_rpm);
  lv_arc_set_range(arc_rpm, 0, MAX_RPM);
  lv_arc_set_value(arc_rpm, 0);
  lv_arc_set_bg_angles(arc_rpm, ARC_START_ANGLE, ARC_END_ANGLE);
  lv_arc_set_rotation(arc_rpm, 0);
  lv_obj_remove_style(arc_rpm, NULL, LV_PART_KNOB);
  lv_obj_set_style_arc_color(arc_rpm, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc_rpm, ARC_WIDTH_DEFAULT, LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc_rpm, COLOR_ACCENT, LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(arc_rpm, ARC_WIDTH_DEFAULT, LV_PART_INDICATOR);
  lv_obj_remove_flag(arc_rpm, LV_OBJ_FLAG_CLICKABLE);

  // --- Connection status ---
  lbl_status = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_status, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_status, COLOR_SUBLABEL, 0);
  lv_label_set_text(lbl_status, LV_SYMBOL_CLOSE " Disconnected");
  lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, TITLE_Y + 20);

  // --- Speed ---
  lbl_speed = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_speed, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_speed, COLOR_WHITE, 0);
  lv_label_set_text(lbl_speed, "---");
  lv_obj_align(lbl_speed, LV_ALIGN_CENTER, 0, -20);

  lv_obj_t* lbl_speed_unit = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_speed_unit, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_speed_unit, COLOR_SUBLABEL, 0);
  lv_label_set_text(lbl_speed_unit, "km/h");
  lv_obj_align(lbl_speed_unit, LV_ALIGN_CENTER, 0, 15);

  // --- RPM value ---
  lbl_rpm = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_rpm, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_rpm, COLOR_WHITE, 0);
  lv_label_set_text(lbl_rpm, "---- RPM");
  lv_obj_align(lbl_rpm, LV_ALIGN_CENTER, 0, 45);

  // --- Coolant temp ---
  lv_obj_t* lbl_cool_icon = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_cool_icon, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_cool_icon, COLOR_LABEL, 0);
  lv_label_set_text(lbl_cool_icon, "COOL");
  lv_obj_align(lbl_cool_icon, LV_ALIGN_CENTER, -50, 75);

  lbl_coolant = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_coolant, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_coolant, COLOR_WHITE, 0);
  lv_label_set_text(lbl_coolant, "--\u00B0");
  lv_obj_align(lbl_coolant, LV_ALIGN_CENTER, -50, 95);

  // --- Boost ---
  lv_obj_t* lbl_boost_icon = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_boost_icon, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_boost_icon, COLOR_LABEL, 0);
  lv_label_set_text(lbl_boost_icon, "BOOST");
  lv_obj_align(lbl_boost_icon, LV_ALIGN_CENTER, 50, 75);

  lbl_boost = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_boost, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_boost, COLOR_WHITE, 0);
  lv_label_set_text(lbl_boost, "-- bar");
  lv_obj_align(lbl_boost, LV_ALIGN_CENTER, 50, 95);

  // --- Voltage ---
  lbl_voltage = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_voltage, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_voltage, COLOR_LABEL, 0);
  lv_label_set_text(lbl_voltage, "-- V");
  lv_obj_align(lbl_voltage, LV_ALIGN_CENTER, 0, 114);
}

void screen_bmw_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data)
    return;

  char buf[32];

  // Update connection status
  if (lbl_status) {
    if (data->connected) {
      lv_obj_set_style_text_color(lbl_status, COLOR_OK, 0);
      lv_label_set_text(lbl_status, LV_SYMBOL_OK " Connected");
    } else {
      lv_obj_set_style_text_color(lbl_status, COLOR_ERROR, 0);
      lv_label_set_text(lbl_status, LV_SYMBOL_CLOSE " Disconnected");
    }
  }

  // Speed
  if (lbl_speed) {
    snprintf(buf, sizeof(buf), "%d", data->speed_kmh);
    lv_label_set_text(lbl_speed, buf);
  }

  // RPM + arc
  if (lbl_rpm) {
    snprintf(buf, sizeof(buf), "%d RPM", data->rpm);
    lv_label_set_text(lbl_rpm, buf);
  }
  if (arc_rpm) {
    lv_arc_set_value(arc_rpm, data->rpm);
    // Color shifts red above 5500 RPM
    if (data->rpm > 5500) {
      lv_obj_set_style_arc_color(arc_rpm, COLOR_ERROR, LV_PART_INDICATOR);
    } else if (data->rpm > 3500) {
      lv_obj_set_style_arc_color(arc_rpm, COLOR_WARN, LV_PART_INDICATOR);
    } else {
      lv_obj_set_style_arc_color(arc_rpm, COLOR_ACCENT, LV_PART_INDICATOR);
    }
  }

  // Coolant
  if (lbl_coolant) {
    snprintf(buf, sizeof(buf), "%d\u00B0", data->coolant_temp);
    lv_label_set_text(lbl_coolant, buf);
    // Red if overheating
    if (data->coolant_temp > 105) {
      lv_obj_set_style_text_color(lbl_coolant, COLOR_ERROR, 0);
    } else {
      lv_obj_set_style_text_color(lbl_coolant, COLOR_WARN, 0);
    }
  }

  // Boost
  if (lbl_boost) {
    snprintf(buf, sizeof(buf), "%.1f", (double)data->boost_bar);
    lv_label_set_text(lbl_boost, buf);
  }

  // Voltage
  if (lbl_voltage) {
    snprintf(buf, sizeof(buf), "%.1fV", (double)data->voltage);
    lv_label_set_text(lbl_voltage, buf);
  }
}

void screen_bmw_destroy(void) {
  lbl_speed = NULL;
  lbl_rpm = NULL;
  lbl_coolant = NULL;
  lbl_boost = NULL;
  lbl_voltage = NULL;
  lbl_status = NULL;
  arc_rpm = NULL;
}
