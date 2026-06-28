#include <Arduino.h>
#include "design.h"
#include "obd2_provider.h"
#include "screens.h"

// DTC screen: List of trouble codes with count header
static lv_obj_t* lbl_count = NULL;
static lv_obj_t* lbl_codes[8] = {};

void screen_dtc_create(lv_obj_t* parent) {
  // Count
  lbl_count = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_count, FONT_LG, 0);
  lv_obj_set_style_text_color(lbl_count, COLOR_WHITE, 0);
  lv_label_set_text(lbl_count, "0");
  lv_obj_align(lbl_count, LV_ALIGN_CENTER, 0, -80);

  lv_obj_t* lbl_active = lv_label_create(parent);
  lv_obj_set_style_text_font(lbl_active, FONT_SM, 0);
  lv_obj_set_style_text_color(lbl_active, COLOR_LABEL, 0);
  lv_label_set_text(lbl_active, "ACTIVE FAULTS");
  lv_obj_align(lbl_active, LV_ALIGN_CENTER, 0, -55);

  // Code list (up to 8 visible, spaced evenly)
  for (int i = 0; i < 8; i++) {
    lbl_codes[i] = lv_label_create(parent);
    lv_obj_set_style_text_font(lbl_codes[i], FONT_MD, 0);
    lv_obj_set_style_text_color(lbl_codes[i], COLOR_WHITE, 0);
    lv_label_set_text(lbl_codes[i], "");
    lv_obj_align(lbl_codes[i], LV_ALIGN_CENTER, 0, -24 + i * 20);
  }
}

void screen_dtc_update(void) {
  const OBD2Data* d = obd2_get_data();
  if (!d)
    return;
  char buf[24];

  if (lbl_count) {
    snprintf(buf, sizeof(buf), "%d", d->dtc_count);
    lv_label_set_text(lbl_count, buf);
  }

  for (int i = 0; i < 8; i++) {
    if (!lbl_codes[i])
      continue;
    if (i < d->dtc_count && d->dtc_codes[i] != 0) {
      // Format as P0XXX (generic OBD2 format)
      uint32_t code = d->dtc_codes[i];
      snprintf(buf, sizeof(buf), "P%04X", (unsigned int)(code & 0xFFFF));
      lv_label_set_text(lbl_codes[i], buf);
    } else {
      lv_label_set_text(lbl_codes[i], "");
    }
  }
}

void screen_dtc_destroy(void) {
  lbl_count = NULL;
  for (int i = 0; i < 8; i++)
    lbl_codes[i] = NULL;
}
