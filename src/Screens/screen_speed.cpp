#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Ultra-minimal: just speed number, nothing else
static lv_obj_t* lbl_speed_big = NULL;

void screen_speed_create(lv_obj_t* parent) {
  // Single giant speed number
  lbl_speed_big = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_speed_big, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_speed_big, COLOR_WHITE, 0);
  lv_label_set_text(lbl_speed_big, "0");
  lv_obj_center(lbl_speed_big);

  // Tiny unit label
  lv_obj_t* lbl_unit = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_unit, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_unit, COLOR_SUBLABEL, 0);
  lv_label_set_text(lbl_unit, "km/h");
  lv_obj_align(lbl_unit, LV_ALIGN_CENTER, 0, 35);
}

void screen_speed_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data || !lbl_speed_big)
    return;
  char buf[8];
  snprintf(buf, sizeof(buf), "%d", data->speed_kmh);
  lv_label_set_text(lbl_speed_big, buf);
}

void screen_speed_destroy(void) {
  lbl_speed_big = NULL;
}
