#ifndef PWM_H
#define PWM_H

#include "boards.h"
#include "nrfx_systick.h"
#include "logs.h"

#include <string.h>
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrfx_pwm.h"
#include "nrf_pwm.h"

typedef enum
{
    OFF,
    SLOW_BLINK,
    FAST_BLINK,
    ON
} my_pwm_modes_t;

void pwm_rgb_update(uint32_t, uint32_t, uint32_t);

void pwm_indicator_update(my_pwm_modes_t);

void pwm_init(void);

#endif // PWM_H