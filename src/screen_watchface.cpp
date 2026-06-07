#include <Arduino.h>
#include "design.h"
#include "rtc.h"
#include "screens.h"

// --- UI Elements ---
static lv_obj_t* lbl_time = NULL;
static lv_obj_t* lbl_date = NULL;
static lv_obj_t* lbl_seconds = NULL;

static uint8_t last_sec = 0xFF;  // Force update on first frame

static const char* day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char* month_names[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void screen_watchface_create(lv_obj_t* parent) {
  // --- Time (center, large) ---
  lbl_time = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_time, FONT_XL, 0);
  lv_obj_set_style_text_color(lbl_time, COLOR_WHITE, 0);
  lv_label_set_text(lbl_time, "--:--");
  lv_obj_align(lbl_time, LV_ALIGN_CENTER, 0, -20);

  // --- Seconds (aligned to right edge of time label to avoid overlap) ---
  lbl_seconds = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_seconds, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_seconds, COLOR_WHITE, 0);
  lv_label_set_text(lbl_seconds, ":00");
  lv_obj_align_to(lbl_seconds, lbl_time, LV_ALIGN_OUT_RIGHT_MID, 4, 0);

  // --- Date ---
  lbl_date = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_date, FONT_MD, 0);
  lv_obj_set_style_text_color(lbl_date, COLOR_WHITE, 0);
  lv_label_set_text(lbl_date, "---");
  lv_obj_align(lbl_date, LV_ALIGN_CENTER, 0, 20);

  last_sec = 0xFF;  // Force immediate update
}

void screen_watchface_update(void) {
  const RTCTime* t = rtc_get_time();
  if (!t)
    return;

  // Only update UI when second changes
  if (t->second == last_sec)
    return;
  last_sec = t->second;

  if (lbl_time) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", t->hour, t->minute);
    lv_label_set_text(lbl_time, buf);
  }
  if (lbl_seconds) {
    char buf[8];
    snprintf(buf, sizeof(buf), ":%02d", t->second);
    lv_label_set_text(lbl_seconds, buf);
    lv_obj_align_to(lbl_seconds, lbl_time, LV_ALIGN_OUT_RIGHT_MID, 4, 0);
  }
  if (lbl_date) {
    char buf[32];
    uint8_t wd = (t->weekday < 7) ? t->weekday : 0;
    uint8_t mo = (t->month >= 1 && t->month <= 12) ? t->month : 1;
    snprintf(buf, sizeof(buf), "%s, %s %d", day_names[wd], month_names[mo], t->day);
    lv_label_set_text(lbl_date, buf);
  }
}

void screen_watchface_destroy(void) {
  lbl_time = NULL;
  lbl_date = NULL;
  lbl_seconds = NULL;
}
