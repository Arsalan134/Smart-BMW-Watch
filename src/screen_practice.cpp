#include <Arduino.h>
#include <lvgl.h>
#include "design.h"
#include "images.h"
#include "screens.h"

#define PRACTICE_TICK_LABEL_OFFSET 12

static lv_obj_t* gauge = NULL;
static lv_obj_t* needle_line = NULL;
static lv_obj_t* speed_overlay = NULL;
static lv_obj_t* speed_label = NULL;

static int demo_speed = 0;
static int demo_dir = 1;
static uint32_t last_update_ms = 0;

static void set_needle_line_value(int32_t v) {
  if (!gauge || !needle_line)
    return;

  lv_scale_set_line_needle_value(gauge, needle_line, 90, v);
}

static void apply_screen_empty_background(lv_obj_t* parent) {
  // Create full-screen background image
  lv_obj_t* bg_img = lv_image_create(parent);
  lv_image_set_src(bg_img, &back);
  lv_obj_set_size(bg_img, DISPLAY_SIZE, DISPLAY_SIZE);
  lv_obj_center(bg_img);
}

void screen_practice_create(lv_obj_t* parent) {
  apply_screen_empty_background(parent);

  gauge = lv_scale_create(parent);
  lv_obj_set_size(gauge, 360, 360);
  lv_scale_set_mode(gauge, LV_SCALE_MODE_ROUND_INNER);
  lv_obj_set_style_bg_opa(gauge, LV_OPA_TRANSP, 0);
  lv_obj_center(gauge);
  lv_scale_set_label_show(gauge, true);
  lv_scale_set_total_tick_count(gauge, 27);
  lv_scale_set_major_tick_every(gauge, 2);
  lv_obj_set_style_length(gauge, 8, LV_PART_ITEMS);
  lv_obj_set_style_length(gauge, 14, LV_PART_INDICATOR);
  lv_scale_set_range(gauge, 0, 260);
  lv_scale_set_angle_range(gauge, 270);
  lv_scale_set_rotation(gauge, 135);
  lv_obj_set_style_text_color(gauge, COLOR_WHITE, LV_PART_INDICATOR);
  lv_obj_set_style_pad_radial(gauge, PRACTICE_TICK_LABEL_OFFSET, LV_PART_INDICATOR);
  lv_obj_set_style_line_color(gauge, COLOR_WHITE, LV_PART_ITEMS);
  lv_obj_set_style_line_color(gauge, COLOR_WHITE, LV_PART_INDICATOR);

  needle_line = lv_line_create(gauge);
  lv_obj_set_style_line_width(needle_line, 4, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);
  lv_obj_set_style_line_color(needle_line, COLOR_WHITE, LV_PART_MAIN);

  speed_overlay = lv_obj_create(parent);
  lv_obj_set_size(speed_overlay, 140, 140);
  lv_obj_center(speed_overlay);
  lv_obj_set_style_radius(speed_overlay, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(speed_overlay, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(speed_overlay, LV_OPA_COVER, 0);
  lv_obj_set_style_border_width(speed_overlay, 2, 0);
  lv_obj_set_style_border_color(speed_overlay, COLOR_WHITE, 0);
  lv_obj_set_style_pad_all(speed_overlay, 0, 0);
  lv_obj_set_scrollbar_mode(speed_overlay, LV_SCROLLBAR_MODE_OFF);

  speed_label = lv_label_create(speed_overlay);
  lv_obj_set_style_text_font(speed_label, FONT_XL, 0);
  lv_obj_set_style_text_color(speed_label, COLOR_WHITE, 0);
  lv_label_set_text(speed_label, "0");
  lv_obj_center(speed_label);

  demo_speed = 0;
  demo_dir = 1;
  last_update_ms = millis();
  set_needle_line_value(demo_speed);
}

void screen_practice_update(void) {
  if (!gauge || !needle_line || !speed_label)
    return;

  uint32_t now = millis();
  if (now - last_update_ms < 40)
    return;
  last_update_ms = now;

  demo_speed += (demo_dir * 2);
  if (demo_speed >= 260) {
    demo_speed = 260;
    demo_dir = -1;

  } else if (demo_speed <= 0) {
    demo_speed = 0;
    demo_dir = 1;
  }

  set_needle_line_value(demo_speed);

  char buf[8];
  snprintf(buf, sizeof(buf), "%d", demo_speed);
  lv_label_set_text(speed_label, buf);
}

void screen_practice_destroy(void) {
  gauge = NULL;
  needle_line = NULL;
  speed_overlay = NULL;
  speed_label = NULL;
}
