#ifndef PWM_H
#define PWM_H

#include "stdint.h"

typedef enum
{
    OFF,
    SLOW_BLINK,
    FAST_BLINK,
    ON
} pwm_indicator_modes_t;

void pwm_init(void);

void pwm_rgb_update(uint32_t, uint32_t, uint32_t);

void pwm_indicator_update(pwm_indicator_modes_t);

#endif // PWM_H