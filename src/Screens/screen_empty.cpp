#include <Arduino.h>
#include <lvgl.h>
#include "design.h"
#include "images.h"
#include "screens.h"

static lv_obj_t* needle_line = NULL;
static lv_obj_t* gauge = NULL;
static lv_obj_t* speed_overlay = NULL;
static lv_obj_t* speed_label = NULL;

static int rpm = 0;
static int demo_dir = 1;
static uint32_t last_update_ms = 0;

// Mock gauge settings - adjust these to match your background gauge image
#define GAUGE_MIN_VALUE 0
#define GAUGE_MAX_VALUE 8000
#define GAUGE_START_ANGLE 135  // degrees (bottom-left)
#define GAUGE_ANGLE_RANGE 270  // degrees of rotation
#define NEEDLE_LENGTH 140      // pixels from center

static lv_point_precise_t needle_points[2];

static void set_needle_line_value(int32_t value) {
  if (!needle_line)
    return;

  lv_scale_set_line_needle_value(gauge, needle_line, 180, value);
}

static void apply_screen_empty_background(lv_obj_t* parent) {
  // Create full-screen background image
  lv_obj_t* bg_img = lv_image_create(parent);
  lv_image_set_src(bg_img, &bmw_tacho);
  lv_obj_set_size(bg_img, DISPLAY_SIZE, DISPLAY_SIZE);
  lv_obj_center(bg_img);
  lv_obj_move_to_index(bg_img, 0);
}

void screen_empty_create(lv_obj_t* parent) {
  apply_screen_empty_background(parent);

  gauge = lv_scale_create(parent);
  lv_obj_set_size(gauge, 360, 360);
  lv_scale_set_mode(gauge, LV_SCALE_MODE_ROUND_INNER);
  lv_obj_set_style_bg_opa(gauge, LV_OPA_TRANSP, 0);
  lv_obj_center(gauge);
  lv_scale_set_label_show(gauge, false);
  lv_scale_set_total_tick_count(gauge, 0);
  lv_scale_set_major_tick_every(gauge, 0);
  lv_obj_set_style_length(gauge, 0, LV_PART_ITEMS);
  lv_obj_set_style_length(gauge, 0, LV_PART_INDICATOR);
  lv_scale_set_range(gauge, GAUGE_MIN_VALUE, GAUGE_MAX_VALUE);
  lv_scale_set_angle_range(gauge, GAUGE_ANGLE_RANGE);
  lv_scale_set_rotation(gauge, GAUGE_START_ANGLE);
  lv_obj_set_style_text_color(gauge, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_set_style_pad_radial(gauge, 12, LV_PART_INDICATOR);
  lv_obj_set_style_line_color(gauge, COLOR_WHITE, LV_PART_ITEMS);
  lv_obj_set_style_line_color(gauge, COLOR_WHITE, LV_PART_INDICATOR);

  needle_line = lv_line_create(gauge);
  lv_obj_set_style_line_width(needle_line, 4, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);
  lv_obj_set_style_line_color(needle_line, COLOR_WHITE, LV_PART_MAIN);
  lv_obj_set_style_outline_pad(needle_line, 50, LV_PART_MAIN);

  // speed_overlay = lv_obj_create(parent);
  // lv_obj_set_size(speed_overlay, 205, 205);
  // lv_obj_center(speed_overlay);
  // lv_obj_set_style_radius(speed_overlay, LV_RADIUS_CIRCLE, 0);
  // lv_obj_set_style_bg_color(speed_overlay, lv_color_make(150, 150, 150), 0);
  // lv_obj_set_style_bg_opa(speed_overlay, LV_OPA_COVER, 0);
  // lv_obj_set_style_border_width(speed_overlay, 0, 0);
  // lv_obj_set_style_border_color(speed_overlay, lv_color_black(), 0);
  // lv_obj_set_style_pad_all(speed_overlay, 0, 0);
  // lv_obj_set_scrollbar_mode(speed_overlay, LV_SCROLLBAR_MODE_OFF);

  speed_label = lv_label_create(speed_overlay);
  lv_obj_set_style_text_font(speed_label, FONT_XL, 0);
  lv_obj_set_style_text_color(speed_label, COLOR_WHITE, 0);
  lv_label_set_text(speed_label, "0");
  lv_obj_center(speed_label);

  // Initialize mock gauge animation
  rpm = 0;
  demo_dir = 1;  // Speed increment
  last_update_ms = millis();
  set_needle_line_value(rpm);
}

void screen_empty_update(void) {
  if (!gauge || !needle_line)
    return;

  uint32_t now = millis();
  if (now - last_update_ms < 40)
    return;
  last_update_ms = now;

  rpm += demo_dir * 100;
  if (rpm >= GAUGE_MAX_VALUE) {
    rpm = GAUGE_MAX_VALUE;
    demo_dir = -1;
  } else if (rpm <= GAUGE_MIN_VALUE) {
    rpm = GAUGE_MIN_VALUE;
    demo_dir = 1;
  }

  set_needle_line_value(rpm);

  char buf[8];
  snprintf(buf, sizeof(buf), "%d", rpm / 10);
  lv_label_set_text(speed_label, buf);
}

void screen_empty_destroy(void) {
  needle_line = NULL;
  gauge = NULL;
  speed_overlay = NULL;
  speed_label = NULL;
}
