#pragma once
#include "lvgl.h"

// =============================================================================
// DESIGN SYSTEM — Single Source of Truth
// All screen files should use these constants for consistent UI.
// =============================================================================

// --- Display Geometry ---
#define DISPLAY_SIZE 360   // px (round, 360x360)
#define SAFE_RADIUS 140    // Safe zone: ±140px from center
#define TITLE_Y -120       // Title label Y offset from center
#define BOTTOM_SAFE_Y 120  // Maximum safe Y before clip

// --- Color Palette ---
// Primary text
#define COLOR_WHITE lv_color_white()
#define COLOR_BLACK lv_color_black()

// Labels/titles (gray accent)
#define COLOR_LABEL COLOR_WHITE
#define COLOR_SUBLABEL lv_color_make(150, 150, 150)

// Accent (BMW blue / cyan)
#define COLOR_ACCENT lv_color_make(0, 150, 255)

// Arc/gauge backgrounds
#define COLOR_ARC_BG lv_color_make(45, 45, 55)

// Status colors
#define COLOR_OK lv_color_make(0, 200, 80)
#define COLOR_WARN lv_color_make(255, 180, 0)
#define COLOR_ERROR lv_color_make(255, 60, 60)

// --- Font Shortcuts (reduced set: 4 sizes) ---
#define FONT_SM &lv_font_montserrat_16  // 16px — Titles, labels, hints, units
#define FONT_MD &lv_font_montserrat_20  // 20px — Body text, secondary values, settings
#define FONT_LG &lv_font_montserrat_36  // 36px — Primary numeric readouts
#define FONT_XL &lv_font_montserrat_48  // 48px — Hero numbers (speed, RPM)

// --- Icon Font (LVGL built-in symbols) ---
#define FONT_ICON &lv_font_montserrat_20  // Use LV_SYMBOL_* constants

// --- Max RPM ---
#define MAX_RPM 7000

// --- Arc/Gauge Defaults ---
#define ARC_WIDTH_DEFAULT 12  // Standard arc stroke width
#define ARC_SWEEP_FULL 270    // Degrees of sweep for full gauges
#define ARC_START_ANGLE 135   // Start angle for 270° sweep arcs
#define ARC_END_ANGLE 405     // End angle (135 + 270)
#define ARC_SIZE_FULL 340     // Full-size arc diameter

// --- Layout Helpers ---
// Standard vertical spacing between label and value
#define SPACING_LABEL_VAL 30  // px between title label and its value
#define SPACING_ROW 35        // px between rows in list views

// --- Transition Animation ---
#define ANIM_DURATION_MS 300  // Screen transition slide duration
