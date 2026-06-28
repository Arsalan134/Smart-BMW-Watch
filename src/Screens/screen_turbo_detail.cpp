#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Turbo Detail: Boost (actual), Boost Target, Manifold Pressure
// Arc surrounds the outer edge, text is inside and readable
static lv_obj_t* arc_boost = NULL;
static lv_obj_t* lbl_boost_val = NULL;
static lv_obj_t* lbl_target = NULL;
static lv_obj_t* lbl_manifold = NULL;

void screen_turbo_detail_create(lv_obj_t* parent) {
  // Boost arc - large, thin ring at outer edge (not overlapping text)
  arc_boost = lv_arc_create(parent);
  lv_obj_set_size(arc_boost, 300, 300);
  lv_arc_set_rotation(arc_boost, 150);
  lv_arc_set_bg_angles(arc_boost, 0, 240);
  lv_arc_set_range(arc_boost, 0, 200);  // 0-2.00 bar * 100
  lv_arc_set_value(arc_boost, 0);
  lv_obj_set_style_arc_width(arc_boost, 10, LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc_boost, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc_boost, 10, LV_PART_INDICATOR);
  lv_obj_set_style_arc_color(arc_boost, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_remove_style(arc_boost, NULL, LV_PART_KNOB);
  lv_obj_remove_flag(arc_boost, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_align(arc_boost, LV_ALIGN_CENTER, 0, 0);

  // Big boost number in center
  lv_obj_t* lbl_unit = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_unit, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_unit, COLOR_LABEL, 0);
  lv_label_set_text(lbl_unit, "BOOST BAR");
  lv_obj_align(lbl_unit, LV_ALIGN_CENTER, 0, -55);

  lbl_boost_val = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_boost_val, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_boost_val, COLOR_WHITE, 0);
  lv_label_set_text(lbl_boost_val, "0.00");
  lv_obj_align(lbl_boost_val, LV_ALIGN_CENTER, 0, -20);

  // Target and manifold below center
  lv_obj_t* t2 = lv_label_create(parent);
  lv_obj_set_style_text_font(t2, FONT_SM, 0);
  lv_obj_set_style_text_color(t2, COLOR_LABEL, 0);
  lv_label_set_text(t2, "TARGET");
  lv_obj_align(t2, LV_ALIGN_CENTER, -55, 30);

  lbl_target = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_target, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_target, COLOR_WHITE, 0);
  lv_label_set_text(lbl_target, "--");
  lv_obj_align(lbl_target, LV_ALIGN_CENTER, -55, 52);

  lv_obj_t* t3 = lv_label_create(parent);
  lv_obj_set_style_text_font(t3, FONT_SM, 0);
  lv_obj_set_style_text_color(t3, COLOR_LABEL, 0);
  lv_label_set_text(t3, "MAP kPa");
  lv_obj_align(t3, LV_ALIGN_CENTER, 55, 30);

  lbl_manifold = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_manifold, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_manifold, COLOR_WHITE, 0);
  lv_label_set_text(lbl_manifold, "--");
  lv_obj_align(lbl_manifold, LV_ALIGN_CENTER, 55, 52);
}

void screen_turbo_detail_update(void) {
  const OBD2Data* d = obd2_get_data();
  if (!d)
    return;
  char buf[16];

  if (arc_boost) {
    int val = (int)(d->boost_bar * 100);
    if (val < 0)
      val = 0;
    if (val > 200)
      val = 200;
    lv_arc_set_value(arc_boost, val);
  }
  if (lbl_boost_val) {
    snprintf(buf, sizeof(buf), "%.2f", d->boost_bar);
    lv_label_set_text(lbl_boost_val, buf);
  }
  if (lbl_target) {
    snprintf(buf, sizeof(buf), "%.2f", d->boost_target);
    lv_label_set_text(lbl_target, buf);
  }
  if (lbl_manifold) {
    snprintf(buf, sizeof(buf), "%.0f", d->manifold_pressure);
    lv_label_set_text(lbl_manifold, buf);
  }
}

void screen_turbo_detail_destroy(void) {
  arc_boost = NULL;
  lbl_boost_val = NULL;
  lbl_target = NULL;
  lbl_manifold = NULL;
}