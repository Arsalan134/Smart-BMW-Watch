#include <Arduino.h>
#include "obd2_provider.h"
#include "screens.h"
#include "design.h"

// Turbo/Intake: Boost pressure, Intake temp, Fuel consumption
// Round-safe layout: all content centered within 300px circle
static lv_obj_t* lbl_boost_val = NULL;
static lv_obj_t* lbl_intake_val = NULL;
static lv_obj_t* lbl_fuel_val = NULL;

void screen_pressure_create(lv_obj_t* parent) {
  // Boost pressure
  lv_obj_t* lbl_b = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_b, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_b, COLOR_LABEL, 0);
  lv_label_set_text(lbl_b, "BOOST");
  lv_obj_align(lbl_b, LV_ALIGN_CENTER, 0, -80);

  lbl_boost_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_boost_val, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_boost_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_boost_val, "--");
  lv_obj_align(lbl_boost_val, LV_ALIGN_CENTER, 0, -50);

  // Intake temp
  lv_obj_t* lbl_i = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_i, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_i, COLOR_LABEL, 0);
  lv_label_set_text(lbl_i, "INTAKE");
  lv_obj_align(lbl_i, LV_ALIGN_CENTER, 0, 0);

  lbl_intake_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_intake_val, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_intake_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_intake_val, "--");
  lv_obj_align(lbl_intake_val, LV_ALIGN_CENTER, 0, 30);

  // Fuel rate
  lv_obj_t* lbl_f = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_f, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_f, COLOR_LABEL, 0);
  lv_label_set_text(lbl_f, "FUEL");
  lv_obj_align(lbl_f, LV_ALIGN_CENTER, 0, 80);

  lbl_fuel_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_fuel_val, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_fuel_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_fuel_val, "--");
  lv_obj_align(lbl_fuel_val, LV_ALIGN_CENTER, 0, 110);
}

void screen_pressure_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data)
    return;
  char buf[16];

  if (lbl_boost_val) {
    snprintf(buf, sizeof(buf), "%.2f bar", (double)data->boost_bar);
    lv_label_set_text(lbl_boost_val, buf);
  }
  if (lbl_intake_val) {
    snprintf(buf, sizeof(buf), "%d\u00B0C", data->intake_temp);
    lv_label_set_text(lbl_intake_val, buf);
  }
  if (lbl_fuel_val) {
    snprintf(buf, sizeof(buf), "%.1f L/h", (double)data->fuel_rate_lph);
    lv_label_set_text(lbl_fuel_val, buf);
  }
}

void screen_pressure_destroy(void) {
  lbl_boost_val = NULL;
  lbl_intake_val = NULL;
  lbl_fuel_val = NULL;
}
