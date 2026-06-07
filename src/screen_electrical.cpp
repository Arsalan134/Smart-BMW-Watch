#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Electrical: Battery voltage + fuel rate (electrical system health)
// Round-safe layout
static lv_obj_t* lbl_volt_val = NULL;
static lv_obj_t* lbl_fuel_val = NULL;

void screen_electrical_create(lv_obj_t* parent) {
  // Battery voltage
  lv_obj_t* lbl_v = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_v, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_v, COLOR_LABEL, 0);
  lv_label_set_text(lbl_v, "BATTERY");
  lv_obj_align(lbl_v, LV_ALIGN_CENTER, 0, -55);

  lbl_volt_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_volt_val, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_volt_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_volt_val, "--");
  lv_obj_align(lbl_volt_val, LV_ALIGN_CENTER, 0, -15);

  // Fuel consumption
  lv_obj_t* lbl_f = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_f, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_f, COLOR_LABEL, 0);
  lv_label_set_text(lbl_f, "FUEL RATE");
  lv_obj_align(lbl_f, LV_ALIGN_CENTER, 0, 45);

  lbl_fuel_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_fuel_val, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_fuel_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_fuel_val, "--");
  lv_obj_align(lbl_fuel_val, LV_ALIGN_CENTER, 0, 85);
}

void screen_electrical_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data)
    return;
  char buf[16];

  if (lbl_volt_val) {
    snprintf(buf, sizeof(buf), "%.1fV", (double)data->voltage);
    lv_label_set_text(lbl_volt_val, buf);
  }
  if (lbl_fuel_val) {
    snprintf(buf, sizeof(buf), "%.1f L/h", (double)data->fuel_rate_lph);
    lv_label_set_text(lbl_fuel_val, buf);
  }
}

void screen_electrical_destroy(void) {
  lbl_volt_val = NULL;
  lbl_fuel_val = NULL;
}
