/**
 * LVGL configuration for Waveshare ESP32-S3 1.85" Round LCD
 * 360x360, ST77916 QSPI, No Touch
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Color depth: 16-bit RGB565 */
#define LV_COLOR_DEPTH 16

/* Disable ARM assembly optimizations (not compatible with Xtensa/ESP32) */
#define LV_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE

/* Memory */
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (64 * 1024U)

/* Display */
#define LV_DPI_DEF 195

/* Refresh period in ms. Lower = higher FPS (default is 33ms ~= 30 FPS). */
#define LV_DEF_REFR_PERIOD 16

#define LV_THEME_DEFAULT_DARK 1
#define LV_USE_THEME_SIMPLE 1

/* Draw */
#define LV_DRAW_SW_SUPPORT_RGB565 1
#define LV_DRAW_SW_COMPLEX 1
#define LV_DRAW_BUF_ALIGN 4

/* OS */
#define LV_USE_OS LV_OS_NONE
#define LV_TICK_CUSTOM 1

/* Logging */
#define LV_USE_LOG 0

/* Fonts */
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 0
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_FONT_DEFAULT &lv_font_montserrat_20

/* Widgets */
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_BTN 1
#define LV_USE_BTNMATRIX 1
#define LV_USE_CANVAS 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_IMAGE 1
#define LV_USE_LABEL 1
#define LV_USE_LINE 1
#define LV_USE_ROLLER 1
#define LV_USE_SCALE 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_TABLE 1
#define LV_USE_TEXTAREA 1

/* Extra widgets */
#define LV_USE_ANIMIMG 1
#define LV_USE_CHART 1
#define LV_USE_LED 1
#define LV_USE_METER 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1

#endif /* LV_CONF_H */
