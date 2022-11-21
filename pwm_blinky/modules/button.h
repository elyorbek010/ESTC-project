#ifndef BUTTON_H
#define BUTTON_H

#include "nrfx_gpiote.h"
#include "boards.h"
#include "app_timer.h"
#include "drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrfx_systick.h"
#include "logs.h"

typedef enum
{
    RELEASED,
    PUSHED
} my_button_state_t;

typedef enum
{
    IDLE,
    CLICKED,
    PRESSED,
    DOUBLE_CLICKED
} my_button_status_t;

typedef struct
{
    nrfx_gpiote_pin_t pin;
    uint32_t idx;
    uint32_t clicks_n;
    my_button_state_t state;
    my_button_status_t status;
} my_button_t;

bool is_clicked(void);
bool is_pressed(void);
bool is_double_clicked(void);

void timer_init(void);
void gpiote_init(void);

#endif // BUTTON_H