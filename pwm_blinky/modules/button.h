#ifndef BUTTON_H
#define BUTTON_H

#include "nrfx_gpiote.h"
#include "boards.h"
#include "app_timer.h"
#include "drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrfx_systick.h"

#include "hsv_rgb.h"

#define btn_pressed(idx) bsp_board_button_state_get(idx)
#define DEBOUNCE_DELAY  50
#define DOUBLE_CLICK_DELAY  500

bool btn_pressed;
bool btn_double_click;
uint8_t btn_presses_n;

void timer_init(void);

void gpiote_init(void);

#endif // BUTTON_H