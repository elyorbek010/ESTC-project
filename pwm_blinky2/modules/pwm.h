#ifndef PWM_H
#define PWM_H

#include "boards.h"
#include "nrfx_systick.h"
#include "logs.h"

#include <string.h>
#include "nrf_drv_pwm.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#include "nrfx_pwm.h"
#include "nrf_pwm.h"

#define CYCLE_PERIOD 1000 // T=1000us = 1ms,  Freq = 1/T = 1kHz

bool duty_cycle_up;
uint32_t duty_cycle;
uint32_t t_on;

/**
 * @brief Blink LEDs sequentially with defined duty cycle
 *
 * @param t_on_ LED on time
 * @param t_period LED modulation period – LED ON time + LED OFF time
 */
//void pwm_blink(uint32_t t_on_, uint32_t t_period);

/**
 * @brief Fluctuate duty cycle 0-100-0 percent
 *
 * @param void no parameters needed
 */
//void fluctuate_duty_cycle();

void pwm0(uint16_t brightness_r, uint16_t brightness_g, uint16_t brightness_b);

void pwm1(uint8_t mode);

void pwm_init(void);

#endif // PWM_H