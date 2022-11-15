#ifndef PWM_H
#define PWM_H

#include "boards.h"
#include "nrfx_systick.h"

#define DEVICE_ID 7201
#define CYCLE_PERIOD  1000 // T=1000us = 1ms,  Freq = 1/T = 1kHz

bool duty_cycle_up;
uint32_t duty_cycle;
uint32_t t_on;

/**
 * @brief Blink LEDs sequentially with defined duty cycle
 *
 * @param t_on_ LED on time
 * @param t_period LED modulation period – LED ON time + LED OFF time
 */
void pwm_blink(uint32_t t_on_, uint32_t t_period);

/**
 * @brief Fluctuate duty cycle 0-100-0 percent
 *
 * @param void no parameters needed
 */
void fluctuate_duty_cycle();

#endif // PWM_H