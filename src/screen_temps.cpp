#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Temperatures: Oil, Coolant, Intake - thermometer style with bars
static lv_obj_t* lbl_oil = NULL;
static lv_obj_t* lbl_cool = NULL;
static lv_obj_t* lbl_intake = NULL;
static lv_obj_t* bar_oil = NULL;
static lv_obj_t* bar_cool = NULL;
static lv_obj_t* bar_intake = NULL;

void screen_temps_create(lv_obj_t* parent) {
  // --- OIL ---
  lv_obj_t* t1 = lv_label_create(parent);
  lv_obj_set_style_text_font(t1, FONT_SM, 0);
  lv_obj_set_style_text_color(t1, COLOR_LABEL, 0);
  lv_label_set_text(t1, "OIL");
  lv_obj_align(t1, LV_ALIGN_CENTER, -70, -80);

  lbl_oil = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_oil, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_oil, COLOR_WHITE, 0);
  lv_label_set_text(lbl_oil, "--");
  lv_obj_align(lbl_oil, LV_ALIGN_CENTER, 30, -80);

  bar_oil = lv_bar_create(parent);
  lv_obj_set_size(bar_oil, 200, 8);
  lv_bar_set_range(bar_oil, 0, 150);
  lv_obj_set_style_bg_color(bar_oil, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_oil, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_align(bar_oil, LV_ALIGN_CENTER, 0, -50);

  // --- COOLANT ---
  lv_obj_t* t2 = lv_label_create(parent);
  lv_obj_set_style_text_font(t2, FONT_SM, 0);
  lv_obj_set_style_text_color(t2, COLOR_LABEL, 0);
  lv_label_set_text(t2, "COOLANT");
  lv_obj_align(t2, LV_ALIGN_CENTER, -70, -10);

  lbl_cool = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_cool, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_cool, COLOR_WHITE, 0);
  lv_label_set_text(lbl_cool, "--");
  lv_obj_align(lbl_cool, LV_ALIGN_CENTER, 30, -10);

  bar_cool = lv_bar_create(parent);
  lv_obj_set_size(bar_cool, 200, 8);
  lv_bar_set_range(bar_cool, 0, 120);
  lv_obj_set_style_bg_color(bar_cool, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_cool, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_align(bar_cool, LV_ALIGN_CENTER, 0, 20);

  // --- INTAKE ---
  lv_obj_t* t3 = lv_label_create(parent);
  lv_obj_set_style_text_font(t3, FONT_SM, 0);
  lv_obj_set_style_text_color(t3, COLOR_LABEL, 0);
  lv_label_set_text(t3, "INTAKE");
  lv_obj_align(t3, LV_ALIGN_CENTER, -70, 60);

  lbl_intake = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_intake, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_intake, COLOR_WHITE, 0);
  lv_label_set_text(lbl_intake, "--");
  lv_obj_align(lbl_intake, LV_ALIGN_CENTER, 30, 60);

  bar_intake = lv_bar_create(parent);
  lv_obj_set_size(bar_intake, 200, 8);
  lv_bar_set_range(bar_intake, 0, 80);
  lv_obj_set_style_bg_color(bar_intake, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_intake, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_align(bar_intake, LV_ALIGN_CENTER, 0, 90);
}

void screen_temps_update(void) {
  const OBD2Data* d = obd2_get_data();
  if (!d)
    return;
  char buf[16];

  if (lbl_oil) {
    snprintf(buf, sizeof(buf), "%d\xC2\xB0", d->oil_temp);
    lv_label_set_text(lbl_oil, buf);
  }
  if (bar_oil)
    lv_bar_set_value(bar_oil, d->oil_temp > 0 ? d->oil_temp : 0, LV_ANIM_ON);

  if (lbl_cool) {
    snprintf(buf, sizeof(buf), "%d\xC2\xB0", d->coolant_temp);
    lv_label_set_text(lbl_cool, buf);
  }
  if (bar_cool)
    lv_bar_set_value(bar_cool, d->coolant_temp > 0 ? d->coolant_temp : 0, LV_ANIM_ON);

  if (lbl_intake) {
    snprintf(buf, sizeof(buf), "%d\xC2\xB0", d->intake_temp);
    lv_label_set_text(lbl_intake, buf);
  }
  if (bar_intake)
    lv_bar_set_value(bar_intake, d->intake_temp > 0 ? d->intake_temp : 0, LV_ANIM_ON);
}

void screen_temps_destroy(void) {
  lbl_oil = NULL;
  lbl_cool = NULL;
  lbl_intake = NULL;
  bar_oil = NULL;
  bar_cool = NULL;
  bar_intake = NULL;
}
