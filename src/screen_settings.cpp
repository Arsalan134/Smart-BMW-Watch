#include <Arduino.h>
#include "design.h"
#include "screens.h"

void screen_settings_create(lv_obj_t* parent) {
  // --- Info items ---
  static const char* items[] = {
      "WiFi: Off",
      "BLE OBD2: Off",
      "Brightness: 100%",
      "Screen Timeout: 30s",
  };

  for (int i = 0; i < 4; i++) {
    lv_obj_t* lbl = lv_label_create(parent);
    lv_obj_set_style_text_font(lbl, FONT_MD, 0);
    lv_obj_set_style_text_color(lbl, COLOR_LABEL, 0);
    lv_label_set_text(lbl, items[i]);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 95 + i * SPACING_ROW);
  }
}

void screen_settings_update(void) {
  // TODO: Update settings values dynamically
}

void screen_settings_destroy(void) {
  // No persistent pointers to clean up
}
