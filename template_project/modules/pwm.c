#include "pwm.h"

uint8_t led_seq[] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 3};
uint8_t led_seq_size = NRFX_ARRAY_SIZE(led_seq);
uint8_t led_index = -1;

void pwm_blink(uint32_t t_on_, uint32_t t_period)
{
    uint32_t t_off_ = t_period - t_on_;

    nrfx_systick_state_t time_cap;
    nrfx_systick_get(&time_cap);

    while (nrfx_systick_test(&time_cap, t_on_) == false)
    {
        bsp_board_led_on(led_seq[led_index]);
    }

    nrfx_systick_get(&time_cap);

    while (nrfx_systick_test(&time_cap, t_off_) == false)
    {
        bsp_board_led_off(led_seq[led_index]);
    }
}

void fluctuate_duty_cycle()
{
    if (t_on == (uint32_t)CYCLE_PERIOD)
    {
        duty_cycle_up = false;
    }
    else if (t_on == 0)
    {
        duty_cycle_up = true;
        led_index++;
        if (led_index == led_seq_size)
        {
            led_index = 0;
        }
    }

    t_on = duty_cycle_up ? (t_on + 1) : (t_on - 1); // Increment if up-flag is set, decrement otherwise

    duty_cycle = t_on * 100 / CYCLE_PERIOD;
}