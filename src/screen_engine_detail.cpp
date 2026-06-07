#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Engine Detail: Load%, Fuel Trims, Lambda, Ignition, Misfire
static lv_obj_t* lbl_load = NULL;
static lv_obj_t* lbl_strim = NULL;
static lv_obj_t* lbl_ltrim = NULL;
static lv_obj_t* lbl_lambda = NULL;
static lv_obj_t* lbl_ign = NULL;
static lv_obj_t* lbl_misfire = NULL;

void screen_engine_detail_create(lv_obj_t* parent) {
  // Engine load - big center value
  lv_obj_t* t1 = lv_label_create(parent);
  lv_obj_set_style_text_font(t1, FONT_SM, 0);
  lv_obj_set_style_text_color(t1, COLOR_LABEL, 0);
  lv_label_set_text(t1, "LOAD");
  lv_obj_align(t1, LV_ALIGN_CENTER, 0, -85);

  lbl_load = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_load, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_load, COLOR_WHITE, 0);
  lv_label_set_text(lbl_load, "--");
  lv_obj_align(lbl_load, LV_ALIGN_CENTER, 0, -50);

  // Fuel trims side by side
  lv_obj_t* t2 = lv_label_create(parent);
  lv_obj_set_style_text_font(t2, FONT_SM, 0);
  lv_obj_set_style_text_color(t2, COLOR_LABEL, 0);
  lv_label_set_text(t2, "S.TRIM");
  lv_obj_align(t2, LV_ALIGN_CENTER, -55, 0);

  lbl_strim = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_strim, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_strim, COLOR_WHITE, 0);
  lv_label_set_text(lbl_strim, "--");
  lv_obj_align(lbl_strim, LV_ALIGN_CENTER, -55, 22);

  lv_obj_t* t3 = lv_label_create(parent);
  lv_obj_set_style_text_font(t3, FONT_SM, 0);
  lv_obj_set_style_text_color(t3, COLOR_LABEL, 0);
  lv_label_set_text(t3, "L.TRIM");
  lv_obj_align(t3, LV_ALIGN_CENTER, 55, 0);

  lbl_ltrim = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_ltrim, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_ltrim, COLOR_WHITE, 0);
  lv_label_set_text(lbl_ltrim, "--");
  lv_obj_align(lbl_ltrim, LV_ALIGN_CENTER, 55, 22);

  // Lambda + ignition
  lv_obj_t* t4 = lv_label_create(parent);
  lv_obj_set_style_text_font(t4, FONT_SM, 0);
  lv_obj_set_style_text_color(t4, COLOR_LABEL, 0);
  lv_label_set_text(t4, "LAMBDA");
  lv_obj_align(t4, LV_ALIGN_CENTER, -55, 60);

  lbl_lambda = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_lambda, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_lambda, COLOR_WHITE, 0);
  lv_label_set_text(lbl_lambda, "--");
  lv_obj_align(lbl_lambda, LV_ALIGN_CENTER, -55, 82);

  lv_obj_t* t5 = lv_label_create(parent);
  lv_obj_set_style_text_font(t5, FONT_SM, 0);
  lv_obj_set_style_text_color(t5, COLOR_LABEL, 0);
  lv_label_set_text(t5, "IGN ADV");
  lv_obj_align(t5, LV_ALIGN_CENTER, 55, 60);

  lbl_ign = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_ign, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_ign, COLOR_WHITE, 0);
  lv_label_set_text(lbl_ign, "--");
  lv_obj_align(lbl_ign, LV_ALIGN_CENTER, 55, 82);

  // Misfire count (bottom center)
  lv_obj_t* t6 = lv_label_create(parent);
  lv_obj_set_style_text_font(t6, FONT_SM, 0);
  lv_obj_set_style_text_color(t6, COLOR_LABEL, 0);
  lv_label_set_text(t6, "MISFIRE");
  lv_obj_align(t6, LV_ALIGN_CENTER, 0, 116);

  lbl_misfire = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_misfire, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_misfire, COLOR_WHITE, 0);
  lv_label_set_text(lbl_misfire, "--");
  lv_obj_align(lbl_misfire, LV_ALIGN_CENTER, 0, 138);
}

void screen_engine_detail_update(void) {
  const OBD2Data* d = obd2_get_data();
  if (!d)
    return;
  char buf[16];

  if (lbl_load) {
    snprintf(buf, sizeof(buf), "%d%%", d->engine_load);
    lv_label_set_text(lbl_load, buf);
  }
  if (lbl_strim) {
    snprintf(buf, sizeof(buf), "%+.1f", d->fuel_trim_short);
    lv_label_set_text(lbl_strim, buf);
  }
  if (lbl_ltrim) {
    snprintf(buf, sizeof(buf), "%+.1f", d->fuel_trim_long);
    lv_label_set_text(lbl_ltrim, buf);
  }
  if (lbl_lambda) {
    snprintf(buf, sizeof(buf), "%.2f", d->lambda);
    lv_label_set_text(lbl_lambda, buf);
  }
  if (lbl_ign) {
    snprintf(buf, sizeof(buf), "%.1f\xC2\xB0", d->ignition_advance);
    lv_label_set_text(lbl_ign, buf);
  }
  if (lbl_misfire) {
    snprintf(buf, sizeof(buf), "%u", d->misfire_count);
    lv_label_set_text(lbl_misfire, buf);
  }
}

void screen_engine_detail_destroy(void) {
  lbl_load = NULL;
  lbl_strim = NULL;
  lbl_ltrim = NULL;
  lbl_lambda = NULL;
  lbl_ign = NULL;
  lbl_misfire = NULL;
}
