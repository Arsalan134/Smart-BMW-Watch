#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Minimal RPM-focused screen: big RPM number + thin arc
static lv_obj_t* lbl_rpm_big = NULL;
static lv_obj_t* arc_rpm_min = NULL;
static lv_obj_t* lbl_gear_hint = NULL;
static lv_obj_t* lbl_speed_top = NULL;

static const char* gear_str(uint8_t g) {
  switch (g) {
    case 0:
      return "N";
    case 7:
      return "R";
    default: {
      static char gb[2];
      gb[0] = (g >= 1 && g <= 9) ? ('0' + g) : '?';
      gb[1] = 0;
      return gb;
    }
  }
}

void screen_rpm_create(lv_obj_t* parent) {
  // Speed
  lbl_speed_top = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_speed_top, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_speed_top, COLOR_WHITE, 0);
  lv_label_set_text(lbl_speed_top, "0");
  lv_obj_align(lbl_speed_top, LV_ALIGN_CENTER, 0, -90);

  lv_obj_t* lbl_speed_unit = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_speed_unit, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_speed_unit, COLOR_SUBLABEL, 0);
  lv_label_set_text(lbl_speed_unit, "km/h");
  lv_obj_align(lbl_speed_unit, LV_ALIGN_CENTER, 0, -60);

  // Thin RPM arc (within safe circular area)
  arc_rpm_min = lv_arc_create(parent);
  lv_obj_set_size(arc_rpm_min, 320, 320);
  lv_obj_center(arc_rpm_min);
  lv_arc_set_range(arc_rpm_min, 0, MAX_RPM);
  lv_arc_set_value(arc_rpm_min, 0);
  lv_arc_set_bg_angles(arc_rpm_min, 0, 360);
  lv_arc_set_rotation(arc_rpm_min, 270);
  lv_obj_remove_style(arc_rpm_min, NULL, LV_PART_KNOB);
  lv_obj_set_style_arc_color(arc_rpm_min, COLOR_ARC_BG, LV_PART_MAIN);
  lv_obj_set_style_arc_width(arc_rpm_min, 5, LV_PART_MAIN);
  lv_obj_set_style_arc_color(arc_rpm_min, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_set_style_arc_width(arc_rpm_min, 5, LV_PART_INDICATOR);
  lv_obj_remove_flag(arc_rpm_min, LV_OBJ_FLAG_CLICKABLE);

  // Big RPM number
  lbl_rpm_big = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_rpm_big, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_rpm_big, COLOR_WHITE, 0);
  lv_label_set_text(lbl_rpm_big, "0");
  lv_obj_align(lbl_rpm_big, LV_ALIGN_CENTER, 0, -10);

  // Label
  lv_obj_t* lbl_unit = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_unit, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_unit, COLOR_SUBLABEL, 0);
  lv_label_set_text(lbl_unit, "RPM");
  lv_obj_align(lbl_unit, LV_ALIGN_CENTER, 0, 25);

  // Gear from direct OBD2 data
  lbl_gear_hint = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_gear_hint, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_gear_hint, COLOR_LABEL, 0);
  lv_label_set_text(lbl_gear_hint, "N");
  lv_obj_align(lbl_gear_hint, LV_ALIGN_CENTER, 0, 70);
}

void screen_rpm_update(void) {
  const OBD2Data* data = obd2_get_data();
  if (!data)
    return;
  char buf[16];

  if (lbl_rpm_big) {
    snprintf(buf, sizeof(buf), "%d", data->rpm);
    lv_label_set_text(lbl_rpm_big, buf);
  }
  if (lbl_speed_top) {
    snprintf(buf, sizeof(buf), "%d", data->speed_kmh);
    lv_label_set_text(lbl_speed_top, buf);
  }
  if (arc_rpm_min) {
    lv_arc_set_value(arc_rpm_min, data->rpm);
  }

  if (lbl_gear_hint) {
    lv_label_set_text(lbl_gear_hint, gear_str(data->gear));
  }
}

void screen_rpm_destroy(void) {
  lbl_rpm_big = NULL;
  arc_rpm_min = NULL;
  lbl_gear_hint = NULL;
  lbl_speed_top = NULL;
}
