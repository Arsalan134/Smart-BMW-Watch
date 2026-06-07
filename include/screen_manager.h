#pragma once
#include "lvgl.h"

// --- Screen IDs ---
enum ScreenID {
  SCREEN_EMPTY = 0,
  SCREEN_PRACTICE,
  SCREEN_WATCHFACE,
  SCREEN_BMW_TELEM,
  SCREEN_SPEED,
  SCREEN_RPM,
  SCREEN_ENGINE,
  SCREEN_ENGINE_DETAIL,
  SCREEN_TEMPS,
  SCREEN_PRESSURE,
  SCREEN_TURBO_DETAIL,
  SCREEN_DRIVING,
  SCREEN_ELECTRICAL,
  SCREEN_BODY,
  SCREEN_PERFORMANCE,
  SCREEN_DTC,
  SCREEN_STATS,
  SCREEN_SETTINGS,
  SCREEN_COUNT  // Always last - total number of screens
};

// --- Screen interface (each screen implements these) ---
typedef struct {
  void (*create)(lv_obj_t* parent);  // Build UI on this parent object
  void (*update)(void);              // Called every loop iteration
  void (*destroy)(void);             // Cleanup when leaving screen
  const char* name;                  // Screen name for debug
} Screen;

// --- Screen Manager API ---
void screen_manager_init(void);
void screen_manager_switch(ScreenID id);
void screen_manager_next(void);
void screen_manager_prev(void);
void screen_manager_update(void);
ScreenID screen_manager_current(void);

// --- Button handling ---
#define BTN_PIN 0  // GPIO0 = BOOT button (side button on Waveshare)
void button_init(void);
void button_update(void);
