#include <Arduino.h>
#include "obd2_provider.h"
#include "screens.h"
#include "design.h"

// Body Status: Doors, Seatbelt, Headlights, Turns, Parking distance
// Icon-style layout with status indicators
static lv_obj_t* lbl_doors = NULL;
static lv_obj_t* lbl_belt = NULL;
static lv_obj_t* lbl_lights = NULL;
static lv_obj_t* lbl_turn_l = NULL;
static lv_obj_t* lbl_turn_r = NULL;
static lv_obj_t* lbl_park = NULL;

static void make_status_row(lv_obj_t* parent, const char* label_text, lv_obj_t** val_lbl, int y_offset) {
  lv_obj_t* lbl = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl, COLOR_LABEL, 0);
  lv_label_set_text(lbl, label_text);
  lv_obj_align(lbl, LV_ALIGN_CENTER, -60, y_offset);

  *val_lbl = lv_label_create(parent);
  lv_obj_set_style_text_font(*val_lbl, FONT_MD, 0);
  lv_obj_set_style_text_color(*val_lbl, COLOR_WHITE, 0);
  lv_label_set_text(*val_lbl, "--");
  lv_obj_align(*val_lbl, LV_ALIGN_CENTER, 50, y_offset);
}

void screen_body_create(lv_obj_t* parent) {
  make_status_row(parent, "DOORS", &lbl_doors, -75);
  make_status_row(parent, "SEATBELT", &lbl_belt, -40);
  make_status_row(parent, "LIGHTS", &lbl_lights, -5);
  make_status_row(parent, "PARK DIST", &lbl_park, 30);

  // Turn signals - left and right with blinking text
  lbl_turn_l = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_turn_l, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_turn_l, COLOR_WHITE, 0);
  lv_label_set_text(lbl_turn_l, "");
  lv_obj_align(lbl_turn_l, LV_ALIGN_CENTER, -60, 85);

  lbl_turn_r = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_turn_r, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_turn_r, COLOR_WHITE, 0);
  lv_label_set_text(lbl_turn_r, "");
  lv_obj_align(lbl_turn_r, LV_ALIGN_CENTER, 60, 85);
}

void screen_body_update(void) {
  const OBD2Data* d = obd2_get_data();
  if (!d)
    return;

  if (lbl_doors) {
    if (d->doors_open == 0) {
      lv_label_set_text(lbl_doors, "ALL SHUT");
    } else {
      // Decode bitmask
      char buf[24] = "";
      if (d->doors_open & 0x01)
        strcat(buf, "FL ");
      if (d->doors_open & 0x02)
        strcat(buf, "FR ");
      if (d->doors_open & 0x04)
        strcat(buf, "RL ");
      if (d->doors_open & 0x08)
        strcat(buf, "RR ");
      if (d->doors_open & 0x10)
        strcat(buf, "TRK");
      lv_label_set_text(lbl_doors, buf);
    }
  }

  if (lbl_belt) {
    lv_label_set_text(lbl_belt, d->seatbelt_driver ? "FASTENED" : "! OPEN !");
  }

  if (lbl_lights) {
    lv_label_set_text(lbl_lights, d->headlights_on ? "ON" : "OFF");
  }

  if (lbl_park) {
    if (d->parking_dist_cm == 255 || d->parking_dist_cm == 0) {
      lv_label_set_text(lbl_park, "CLEAR");
    } else {
      char buf[16];
      snprintf(buf, sizeof(buf), "%d cm", d->parking_dist_cm);
      lv_label_set_text(lbl_park, buf);
    }
  }

  if (lbl_turn_l)
    lv_label_set_text(lbl_turn_l, d->turn_left ? "<< L" : "");
  if (lbl_turn_r)
    lv_label_set_text(lbl_turn_r, d->turn_right ? "R >>" : "");
}

void screen_body_destroy(void) {
  lbl_doors = NULL;
  lbl_belt = NULL;
  lbl_lights = NULL;
  lbl_turn_l = NULL;
  lbl_turn_r = NULL;
  lbl_park = NULL;
}
