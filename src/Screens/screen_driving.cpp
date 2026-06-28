#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// Driving: Gear (huge), Steering angle, Accelerator %, Brake %, DSC, Mode
static lv_obj_t* lbl_gear = NULL;
static lv_obj_t* lbl_steer = NULL;
static lv_obj_t* lbl_accel = NULL;
static lv_obj_t* lbl_brake = NULL;
static lv_obj_t* lbl_mode = NULL;
static lv_obj_t* lbl_dsc = NULL;

static const char* gear_str(uint8_t g) {
  switch (g) {
    case 0:
      return "N";
    case 7:
      return "R";
    default: {
      static char gb[2];
      gb[0] = '0' + g;
      gb[1] = 0;
      return gb;
    }
  }
}

static const char* mode_str(uint8_t m) {
  switch (m) {
    case 0:
      return "COMFORT";
    case 1:
      return "SPORT";
    case 2:
      return "ECO";
    default:
      return "?";
  }
}

void screen_driving_create(lv_obj_t* parent) {
  // Giant gear indicator
  lbl_gear = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_gear, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_gear, COLOR_WHITE, 0);
  lv_label_set_text(lbl_gear, "N");
  lv_obj_align(lbl_gear, LV_ALIGN_CENTER, 0, -80);

  lv_obj_t* t_gear = lv_label_create(parent);
  lv_obj_set_style_text_font(t_gear, FONT_SM, 0);
  lv_obj_set_style_text_color(t_gear, COLOR_LABEL, 0);
  lv_label_set_text(t_gear, "GEAR");
  lv_obj_align(t_gear, LV_ALIGN_CENTER, 0, -40);

  // Steering angle
  lv_obj_t* t_st = lv_label_create(parent);
  lv_obj_set_style_text_font(t_st, FONT_SM, 0);
  lv_obj_set_style_text_color(t_st, COLOR_LABEL, 0);
  lv_label_set_text(t_st, "STEER");
  lv_obj_align(t_st, LV_ALIGN_CENTER, -55, -10);

  lbl_steer = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_steer, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_steer, COLOR_WHITE, 0);
  lv_label_set_text(lbl_steer, "0\xC2\xB0");
  lv_obj_align(lbl_steer, LV_ALIGN_CENTER, -55, 14);

  // Drive mode
  lv_obj_t* t_m = lv_label_create(parent);
  lv_obj_set_style_text_font(t_m, FONT_SM, 0);
  lv_obj_set_style_text_color(t_m, COLOR_LABEL, 0);
  lv_label_set_text(t_m, "MODE");
  lv_obj_align(t_m, LV_ALIGN_CENTER, 55, -10);

  lbl_mode = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_mode, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_mode, COLOR_WHITE, 0);
  lv_label_set_text(lbl_mode, "--");
  lv_obj_align(lbl_mode, LV_ALIGN_CENTER, 55, 14);

  // Accelerator and brake bars
  lv_obj_t* t_a = lv_label_create(parent);
  lv_obj_set_style_text_font(t_a, FONT_SM, 0);
  lv_obj_set_style_text_color(t_a, COLOR_LABEL, 0);
  lv_label_set_text(t_a, "ACCEL");
  lv_obj_align(t_a, LV_ALIGN_CENTER, -55, 50);

  lbl_accel = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_accel, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_accel, COLOR_WHITE, 0);
  lv_label_set_text(lbl_accel, "0%");
  lv_obj_align(lbl_accel, LV_ALIGN_CENTER, -55, 74);

  lv_obj_t* t_b = lv_label_create(parent);
  lv_obj_set_style_text_font(t_b, FONT_SM, 0);
  lv_obj_set_style_text_color(t_b, COLOR_LABEL, 0);
  lv_label_set_text(t_b, "BRAKE");
  lv_obj_align(t_b, LV_ALIGN_CENTER, 55, 50);

  lbl_brake = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_brake, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_brake, COLOR_WHITE, 0);
  lv_label_set_text(lbl_brake, "0%");
  lv_obj_align(lbl_brake, LV_ALIGN_CENTER, 55, 74);

  // DSC indicator
  lbl_dsc = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_dsc, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_dsc, COLOR_LABEL, 0);
  lv_label_set_text(lbl_dsc, "");
  lv_obj_align(lbl_dsc, LV_ALIGN_CENTER, 0, 110);
}

void screen_driving_update(void) {
  const OBD2Data* d = obd2_get_data();
  if (!d)
    return;
  char buf[16];

  if (lbl_gear)
    lv_label_set_text(lbl_gear, gear_str(d->gear));
  if (lbl_steer) {
    snprintf(buf, sizeof(buf), "%d\xC2\xB0", d->steering_angle);
    lv_label_set_text(lbl_steer, buf);
  }
  if (lbl_mode)
    lv_label_set_text(lbl_mode, mode_str(d->drive_mode));
  if (lbl_accel) {
    snprintf(buf, sizeof(buf), "%d%%", d->accel_pedal);
    lv_label_set_text(lbl_accel, buf);
  }
  if (lbl_brake) {
    snprintf(buf, sizeof(buf), "%d%%", d->brake_pressure);
    lv_label_set_text(lbl_brake, buf);
  }
  if (lbl_dsc) {
    lv_label_set_text(lbl_dsc, d->dsc_active ? "! DSC ACTIVE !" : "");
    lv_obj_set_style_text_color(lbl_dsc, COLOR_WHITE, 0);
  }
}

void screen_driving_destroy(void) {
  lbl_gear = NULL;
  lbl_steer = NULL;
  lbl_accel = NULL;
  lbl_brake = NULL;
  lbl_mode = NULL;
  lbl_dsc = NULL;
}
