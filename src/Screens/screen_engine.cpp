#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Engine: RPM, Coolant Temp, Throttle
// Round-safe layout: content stays within ~300px center circle
static lv_obj_t* lbl_rpm_val = NULL;
static lv_obj_t* lbl_cool_val = NULL;
static lv_obj_t* lbl_oil_val = NULL;
static lv_obj_t* lbl_thr_val = NULL;

void screen_engine_create(lv_obj_t* parent) {
  // RPM - top section
  // lv_obj_t* ico_rpm = lv_label_create(parent);
  // lv_obj_set_style_text_font(ico_rpm, FONT_ICON, 0);
  // lv_obj_set_style_text_color(ico_rpm, COLOR_SUBLABEL, 0);
  // lv_label_set_text(ico_rpm, LV_SYMBOL_REFRESH);
  // lv_obj_align(ico_rpm, LV_ALIGN_CENTER, -40, -80);

  lv_obj_t* lbl_rpm_title = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_rpm_title, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_rpm_title, COLOR_LABEL, 0);
  lv_label_set_text(lbl_rpm_title, "RPM");
  lv_obj_align(lbl_rpm_title, LV_ALIGN_CENTER, 0, -80);

  lbl_rpm_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_rpm_val, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_rpm_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_rpm_val, "--");
  lv_obj_align(lbl_rpm_val, LV_ALIGN_CENTER, 0, -50);

  // Coolant + Oil - middle row
  lv_obj_t* lbl_cool_title = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_cool_title, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_cool_title, COLOR_LABEL, 0);
  lv_label_set_text(lbl_cool_title, "COOLANT");
  lv_obj_align(lbl_cool_title, LV_ALIGN_CENTER, -70, 0);

  lbl_cool_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_cool_val, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_cool_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_cool_val, "--");
  lv_obj_align(lbl_cool_val, LV_ALIGN_CENTER, -70, 28);

  lv_obj_t* lbl_oil_title = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_oil_title, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_oil_title, COLOR_LABEL, 0);
  lv_label_set_text(lbl_oil_title, "OIL");
  lv_obj_align(lbl_oil_title, LV_ALIGN_CENTER, 70, 0);

  lbl_oil_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_oil_val, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_oil_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_oil_val, "--");
  lv_obj_align(lbl_oil_val, LV_ALIGN_CENTER, 70, 28);

  // Throttle - bottom (keep within circle)
  lv_obj_t* lbl_thr_title = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_thr_title, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_thr_title, COLOR_LABEL, 0);
  lv_label_set_text(lbl_thr_title, "THROTTLE");
  lv_obj_align(lbl_thr_title, LV_ALIGN_CENTER, 0, 80);

  lbl_thr_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_thr_val, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_thr_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_thr_val, "--");
  lv_obj_align(lbl_thr_val, LV_ALIGN_CENTER, 0, 110);
}

void screen_engine_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data)
    return;
  char buf[16];

  if (lbl_rpm_val) {
    snprintf(buf, sizeof(buf), "%d", data->rpm);
    lv_label_set_text(lbl_rpm_val, buf);
  }
  if (lbl_cool_val) {
    snprintf(buf, sizeof(buf), "%d\u00B0C", data->coolant_temp);
    lv_label_set_text(lbl_cool_val, buf);
  }
  if (lbl_oil_val) {
    snprintf(buf, sizeof(buf), "%d\u00B0C", data->oil_temp);
    lv_label_set_text(lbl_oil_val, buf);
  }
  if (lbl_thr_val) {
    snprintf(buf, sizeof(buf), "%d%%", data->throttle_pct);
    lv_label_set_text(lbl_thr_val, buf);
  }
}

void screen_engine_destroy(void) {
  lbl_rpm_val = NULL;
  lbl_cool_val = NULL;
  lbl_oil_val = NULL;
  lbl_thr_val = NULL;
}
