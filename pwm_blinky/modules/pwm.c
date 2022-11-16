#include "pwm.h"
/*
static uint16_t led_index = -1;

void pwm_blink(uint32_t t_on_, uint32_t t_period)
{
    uint32_t t_off_ = t_period - t_on_;

    nrfx_systick_state_t time_cap;
    nrfx_systick_get(&time_cap);

    while (nrfx_systick_test(&time_cap, t_on_) == false)
    {
        bsp_board_led_on(led_index);
    }

    nrfx_systick_get(&time_cap);

    while (nrfx_systick_test(&time_cap, t_off_) == false)
    {
        bsp_board_led_off(led_index);
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
        if (led_index == LEDS_NUMBER)
        {
            led_index = 0;
        }
    }

    t_on = duty_cycle_up ? (t_on + 1) : (t_on - 1);     //Increment if up-flag is set, decrement otherwise

    duty_cycle = t_on * 100 / CYCLE_PERIOD;
}
*/

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);

#define n_top 10000
#define n_step 100
#define n_step2 25
static uint16_t step = n_top / n_step;
static uint16_t step2 = n_top / n_step2;

nrf_pwm_values_individual_t pwm_seq0[4] = {{1000, 1000, 1000, 1000}};
nrf_pwm_values_common_t pwm_seq1[n_step * 2];
nrf_pwm_values_common_t pwm_seq2[n_step2 * 2];
nrf_pwm_values_common_t pwm_seq3[1];

uint16_t value1 = 0;

nrf_pwm_sequence_t const seq0 = {
    .values.p_individual = pwm_seq0,
    .length = NRF_PWM_VALUES_LENGTH(pwm_seq0),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq1 = {
    .values.p_common = pwm_seq1,
    .length = NRF_PWM_VALUES_LENGTH(pwm_seq1),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq2 = {
    .values.p_common = pwm_seq2,
    .length = NRF_PWM_VALUES_LENGTH(pwm_seq2),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq3 = {
    .values.p_common = pwm_seq3,
    .length = NRF_PWM_VALUES_LENGTH(pwm_seq3),
    .repeats = 0,
    .end_delay = 0};

void pwm0(uint16_t brightness_r, uint16_t brightness_g, uint16_t brightness_b)
{
    NRF_LOG_INFO("LED2_R = %u  LED2_G = %u  , LED2_B = %u", brightness_r, brightness_g, brightness_b);
    pwm_seq0->channel_1 = (uint16_t)(brightness_r * (((float)1000)/255));
    pwm_seq0->channel_2 = (uint16_t)(brightness_g * (((float)1000)/255));
    pwm_seq0->channel_3 = (uint16_t)(brightness_b * (((float)1000)/255));
    nrfx_pwm_simple_playback(&m_pwm0, &seq0, 1, NRF_DRV_PWM_FLAG_LOOP);
}

void pwm1(uint8_t mode)
{
    switch (mode)
    {
    case 1:
        nrfx_pwm_stop(&m_pwm1, true);
        break;
    case 2:
        nrfx_pwm_simple_playback(&m_pwm1, &seq1, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    case 3:
        nrfx_pwm_simple_playback(&m_pwm1, &seq2, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    case 4:
        nrfx_pwm_simple_playback(&m_pwm1, &seq3, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    }
}

void pwm_init(void)
{ // PWM0 - RGB
    nrf_drv_pwm_config_t const config0 = {
        .output_pins = {
            NRFX_PWM_PIN_NOT_USED,
            LED_2 | NRFX_PWM_PIN_INVERTED,
            LED_3 | NRFX_PWM_PIN_INVERTED,
            LED_4 | NRFX_PWM_PIN_INVERTED,
        },
        .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
        .base_clock = (nrf_pwm_clk_t)NRFX_PWM_DEFAULT_CONFIG_BASE_CLOCK,
        .count_mode = (nrf_pwm_mode_t)NRFX_PWM_DEFAULT_CONFIG_COUNT_MODE,
        .top_value = NRFX_PWM_DEFAULT_CONFIG_TOP_VALUE,
        .load_mode = (nrf_pwm_dec_load_t)NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode = (nrf_pwm_dec_step_t)NRFX_PWM_DEFAULT_CONFIG_STEP_MODE,
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config0, NULL));

    // PWM1 - Green LED1
    for (int i = 0; i < n_step; i++)
    {
        value1 += step;
        pwm_seq1[i] = value1;
        pwm_seq1[n_step + i] = n_top - value1;
    }
    value1 = 0;
    for (int i = 0; i < n_step2; i++)
    {
        value1 += step2;
        pwm_seq2[i] = value1;
        pwm_seq2[n_step2 + i] = n_top - value1;
    }
    pwm_seq3[0] = n_top;
    nrf_drv_pwm_config_t const config1 = {
        .output_pins = {
            LED_1 | NRFX_PWM_PIN_INVERTED,
            NRFX_PWM_PIN_NOT_USED,
            NRFX_PWM_PIN_NOT_USED,
            NRFX_PWM_PIN_NOT_USED,
        },
        .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
        .base_clock = (nrf_pwm_clk_t)NRFX_PWM_DEFAULT_CONFIG_BASE_CLOCK,
        .count_mode = (nrf_pwm_mode_t)NRFX_PWM_DEFAULT_CONFIG_COUNT_MODE,
        .top_value = n_top,
        .load_mode = (nrf_pwm_dec_load_t)NRF_PWM_LOAD_COMMON,
        .step_mode = (nrf_pwm_dec_step_t)NRFX_PWM_DEFAULT_CONFIG_STEP_MODE,
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm1, &config1, NULL));
}
