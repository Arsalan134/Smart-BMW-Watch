#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// BMW stats: text-based detailed info layout
static lv_obj_t* lbl_spd = NULL;
static lv_obj_t* lbl_rpm = NULL;
static lv_obj_t* lbl_cool = NULL;
static lv_obj_t* lbl_boost = NULL;
static lv_obj_t* lbl_volt = NULL;
static lv_obj_t* lbl_intake = NULL;
static lv_obj_t* lbl_throttle = NULL;
static lv_obj_t* lbl_fuel = NULL;

static lv_obj_t* make_row(lv_obj_t* parent, const char* label, int y_pos) {
  // Round display safe zone: use center-aligned positioning
  lv_obj_t* lbl_name = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_name, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_name, COLOR_LABEL, 0);
  lv_label_set_text(lbl_name, label);
  lv_obj_align(lbl_name, LV_ALIGN_CENTER, -65, y_pos - 180);

  lv_obj_t* lbl_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_val, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_val, "--");
  lv_obj_align(lbl_val, LV_ALIGN_CENTER, 60, y_pos - 180);
  return lbl_val;
}

void screen_stats_create(lv_obj_t* parent) {
  int y = 90;
  int spacing = 27;
  lbl_spd = make_row(parent, "Speed", y);
  y += spacing;
  lbl_rpm = make_row(parent, "RPM", y);
  y += spacing;
  lbl_cool = make_row(parent, "Coolant", y);
  y += spacing;
  lbl_boost = make_row(parent, "Boost", y);
  y += spacing;
  lbl_volt = make_row(parent, "Battery", y);
  y += spacing;
  lbl_intake = make_row(parent, "Intake", y);
  y += spacing;
  lbl_throttle = make_row(parent, "Throttle", y);
  y += spacing;
  lbl_fuel = make_row(parent, "Fuel", y);
}

void screen_stats_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data)
    return;
  char buf[16];

  if (lbl_spd) {
    snprintf(buf, sizeof(buf), "%d km/h", data->speed_kmh);
    lv_label_set_text(lbl_spd, buf);
  }
  if (lbl_rpm) {
    snprintf(buf, sizeof(buf), "%d", data->rpm);
    lv_label_set_text(lbl_rpm, buf);
  }
  if (lbl_cool) {
    snprintf(buf, sizeof(buf), "%d\u00B0C", data->coolant_temp);
    lv_label_set_text(lbl_cool, buf);
  }
  if (lbl_boost) {
    snprintf(buf, sizeof(buf), "%.2f bar", (double)data->boost_bar);
    lv_label_set_text(lbl_boost, buf);
  }
  if (lbl_volt) {
    snprintf(buf, sizeof(buf), "%.1fV", (double)data->voltage);
    lv_label_set_text(lbl_volt, buf);
  }
  if (lbl_intake) {
    snprintf(buf, sizeof(buf), "%d\u00B0C", data->intake_temp);
    lv_label_set_text(lbl_intake, buf);
  }
  if (lbl_throttle) {
    snprintf(buf, sizeof(buf), "%d%%", data->throttle_pct);
    lv_label_set_text(lbl_throttle, buf);
  }
  if (lbl_fuel) {
    snprintf(buf, sizeof(buf), "%.1f L/h", (double)data->fuel_rate_lph);
    lv_label_set_text(lbl_fuel, buf);
  }
}

void screen_stats_destroy(void) {
  lbl_spd = NULL;
  lbl_rpm = NULL;
  lbl_cool = NULL;
  lbl_boost = NULL;
  lbl_volt = NULL;
  lbl_intake = NULL;
  lbl_throttle = NULL;
  lbl_fuel = NULL;
}
