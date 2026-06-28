#include "screen_manager.h"
#include <Arduino.h>
#include "design.h"
#include "screens.h"

// --- Screen registry ---
static const Screen screens[SCREEN_COUNT] = {
    {screen_empty_create, screen_empty_update, screen_empty_destroy, ""},
    {screen_practice_create, screen_practice_update, screen_practice_destroy, ""},
    {screen_watchface_create, screen_watchface_update, screen_watchface_destroy, ""},
    {screen_bmw_create, screen_bmw_update, screen_bmw_destroy, "BMW OBD2"},
    {screen_speed_create, screen_speed_update, screen_speed_destroy, ""},
    {screen_rpm_create, screen_rpm_update, screen_rpm_destroy, ""},
    {screen_engine_create, screen_engine_update, screen_engine_destroy, "ENGINE"},
    {screen_engine_detail_create, screen_engine_detail_update, screen_engine_detail_destroy, "ENGINE DETAIL"},
    {screen_temps_create, screen_temps_update, screen_temps_destroy, "TEMPERATURES"},
    {screen_pressure_create, screen_pressure_update, screen_pressure_destroy, "TURBO"},
    {screen_turbo_detail_create, screen_turbo_detail_update, screen_turbo_detail_destroy, "TURBO DETAIL"},
    {screen_driving_create, screen_driving_update, screen_driving_destroy, "DRIVING"},
    {screen_electrical_create, screen_electrical_update, screen_electrical_destroy, "ELECTRICAL"},
    {screen_body_create, screen_body_update, screen_body_destroy, "BODY STATUS"},
    {screen_performance_create, screen_performance_update, screen_performance_destroy, "PERFORMANCE"},
    {screen_dtc_create, screen_dtc_update, screen_dtc_destroy, "TROUBLE CODES"},
    {screen_stats_create, screen_stats_update, screen_stats_destroy, "ALL DATA"},
    {screen_settings_create, screen_settings_update, screen_settings_destroy, "SETTINGS"},
};

// --- State ---
static ScreenID current_screen = SCREEN_WATCHFACE;
static lv_obj_t* screen_obj = NULL;

// --- Helper: add title label from registry name ---
static void add_screen_title(lv_obj_t* scr, const char* name) {
  if (name && name[0] != '\0') {
    lv_obj_t* title = lv_label_create(scr);
    lv_obj_set_style_text_font(title, FONT_SM, 0);
    lv_obj_set_style_text_color(title, COLOR_LABEL, 0);
    lv_label_set_text(title, name);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, TITLE_Y);
  }
}

// --- Screen transition animation ---
static void do_screen_transition(ScreenID new_id) {
  if (new_id == current_screen && screen_obj != NULL)
    return;
  if (new_id >= SCREEN_COUNT)
    return;

  // Destroy old screen content
  if (screens[current_screen].destroy) {
    screens[current_screen].destroy();
  }

  // Create new LVGL screen object
  lv_obj_t* new_scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(new_scr, COLOR_BLACK, 0);
  lv_obj_set_scrollbar_mode(new_scr, LV_SCROLLBAR_MODE_OFF);

  // Add title from registry (single source of truth)
  add_screen_title(new_scr, screens[new_id].name);

  // Build the new screen's UI
  screens[new_id].create(new_scr);

  // Animate transition (slide left)
  lv_screen_load_anim(new_scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, ANIM_DURATION_MS, 0, true);

  // or use this.  Use direct screen load for stability on this panel path.
  // lv_screen_load(new_scr);

  screen_obj = new_scr;
  current_screen = new_id;
  Serial.printf("Screen -> %s (%d/%d)\n", screens[current_screen].name,
                current_screen + 1, SCREEN_COUNT);
}

// --- Public API ---
void screen_manager_init(void) {
  // Create and show the first screen
  screen_obj = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen_obj, COLOR_BLACK, 0);
  lv_obj_set_scrollbar_mode(screen_obj, LV_SCROLLBAR_MODE_OFF);
  add_screen_title(screen_obj, screens[SCREEN_WATCHFACE].name);
  screens[SCREEN_WATCHFACE].create(screen_obj);
  lv_screen_load(screen_obj);
  current_screen = SCREEN_WATCHFACE;
  Serial.println("Screen manager initialized");
}

void screen_manager_switch(ScreenID id) {
  do_screen_transition(id);
}

void screen_manager_next(void) {
  ScreenID next = (ScreenID)((current_screen + 1) % SCREEN_COUNT);
  do_screen_transition(next);
}

void screen_manager_prev(void) {
  ScreenID prev = (ScreenID)((current_screen + SCREEN_COUNT - 1) % SCREEN_COUNT);
  do_screen_transition(prev);
}

void screen_manager_update(void) {
  if (screens[current_screen].update) {
    screens[current_screen].update();
  }
}

ScreenID screen_manager_current(void) {
  return current_screen;
}

// --- Button handling (debounced, short press = next screen) ---
static bool btn_last_state = true;  // HIGH = not pressed (pull-up)
static unsigned long btn_press_time = 0;
static bool btn_handled = false;
static unsigned long btn_last_event_ms = 0;

void button_init(void) {
  pinMode(BTN_PIN, INPUT_PULLUP);
  Serial.printf("Button initialized on GPIO%d\n", BTN_PIN);
}

void button_update(void) {
  bool state = digitalRead(BTN_PIN);
  unsigned long now = millis();

  // Falling edge (press) with debounce -> next screen.
  if (state == LOW && btn_last_state == HIGH && (now - btn_last_event_ms) > 60) {
    btn_last_event_ms = now;
    btn_press_time = now;
    btn_handled = true;
    screen_manager_next();
  }

  // Long press detection (>1500ms while still held)
  if (state == LOW && !btn_handled) {
    unsigned long hold_time = now - btn_press_time;
    if (hold_time > 1500) {
      // Long press -> reserved for future (sleep, power off, etc.)
      Serial.println("Long press detected (reserved)");
      btn_handled = true;
    }
  }

  // Clear handled flag once button is released.
  if (state == HIGH && btn_last_state == LOW) {
    btn_handled = false;
  }

  btn_last_state = state;
}
