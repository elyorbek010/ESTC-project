#include "pwm.h"

static nrf_drv_pwm_t pwm_indicator = NRF_DRV_PWM_INSTANCE(0);
static nrf_drv_pwm_t pwm_rgb = NRF_DRV_PWM_INSTANCE(1);

#define TOP_VALUE 10000
#define STEPS_SLOW 100
#define STEPS_FAST 25

static const uint16_t step_slow = TOP_VALUE / STEPS_SLOW;
static const uint16_t step_fast = TOP_VALUE / STEPS_FAST;

nrf_pwm_values_individual_t rgb_seq_vals[3];
nrf_pwm_values_common_t indicator_seq_off_vals[1] = {0};
nrf_pwm_values_common_t indicator_seq_slow_vals[STEPS_SLOW * 2];
nrf_pwm_values_common_t indicator_seq_fast_vals[STEPS_FAST * 2];
nrf_pwm_values_common_t indicator_seq_on_vals[1] = {TOP_VALUE};

nrf_pwm_sequence_t const seq_rgb = {
    .values.p_individual = rgb_seq_vals,
    .length = NRF_PWM_VALUES_LENGTH(rgb_seq_vals),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq_off = {
    .values.p_common = indicator_seq_off_vals,
    .length = NRF_PWM_VALUES_LENGTH(indicator_seq_off_vals),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq_slow = {
    .values.p_common = indicator_seq_slow_vals,
    .length = NRF_PWM_VALUES_LENGTH(indicator_seq_slow_vals),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq_fast = {
    .values.p_common = indicator_seq_fast_vals,
    .length = NRF_PWM_VALUES_LENGTH(indicator_seq_fast_vals),
    .repeats = 0,
    .end_delay = 0};

nrf_pwm_sequence_t const seq_on = {
    .values.p_common = indicator_seq_on_vals,
    .length = NRF_PWM_VALUES_LENGTH(indicator_seq_on_vals),
    .repeats = 0,
    .end_delay = 0};

void pwm_rgb_update(uint32_t red, uint32_t green, uint32_t blue)
{
    // NRF_LOG_INFO("LED2_R = %u  LED2_G = %u  , LED2_B = %u", red, green, blue);
    float scale_factor = NRFX_PWM_DEFAULT_CONFIG_TOP_VALUE / (1.0 * 255);
    rgb_seq_vals->channel_0 = (uint32_t)(red * scale_factor);
    rgb_seq_vals->channel_1 = (uint32_t)(green * scale_factor);
    rgb_seq_vals->channel_2 = (uint32_t)(blue * scale_factor);
    nrfx_pwm_simple_playback(&pwm_rgb, &seq_rgb, 1, NRF_DRV_PWM_FLAG_LOOP);
}

void pwm_indicator_update(my_pwm_modes_t mode)
{
    switch (mode)
    {
    case OFF:
        nrfx_pwm_simple_playback(&pwm_indicator, &seq_off, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    case SLOW_BLINK:
        nrfx_pwm_simple_playback(&pwm_indicator, &seq_slow, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    case FAST_BLINK:
        nrfx_pwm_simple_playback(&pwm_indicator, &seq_fast, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    case ON:
        nrfx_pwm_simple_playback(&pwm_indicator, &seq_on, 1, NRF_DRV_PWM_FLAG_LOOP);
        break;
    }
}

void pwm_init(void)
{ // PWM - RGB
    nrf_drv_pwm_config_t const config_rgb = {
        .output_pins = {
            LED_2 | NRFX_PWM_PIN_INVERTED,
            LED_3 | NRFX_PWM_PIN_INVERTED,
            LED_4 | NRFX_PWM_PIN_INVERTED,
            NRFX_PWM_PIN_NOT_USED},
        .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
        .base_clock = (nrf_pwm_clk_t)NRFX_PWM_DEFAULT_CONFIG_BASE_CLOCK,
        .count_mode = (nrf_pwm_mode_t)NRFX_PWM_DEFAULT_CONFIG_COUNT_MODE,
        .top_value = NRFX_PWM_DEFAULT_CONFIG_TOP_VALUE,
        .load_mode = (nrf_pwm_dec_load_t)NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode = (nrf_pwm_dec_step_t)NRFX_PWM_DEFAULT_CONFIG_STEP_MODE,
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&pwm_rgb, &config_rgb, NULL));

    // PWM indicator
    indicator_seq_slow_vals[STEPS_SLOW] = TOP_VALUE;
    for (uint32_t i = 1; i < STEPS_SLOW; i++)
    {
        indicator_seq_slow_vals[i] = indicator_seq_slow_vals[i-1] + step_slow;
        indicator_seq_slow_vals[STEPS_SLOW + i] = TOP_VALUE - indicator_seq_slow_vals[i];
    }

    indicator_seq_fast_vals[STEPS_FAST] = TOP_VALUE;
    for (uint32_t i = 1; i < STEPS_FAST; i++)
    {
        indicator_seq_fast_vals[i] = indicator_seq_fast_vals[i-1] + step_fast;
        indicator_seq_fast_vals[STEPS_FAST + i] = TOP_VALUE - indicator_seq_fast_vals[i];
    }

    nrf_drv_pwm_config_t const config_indicator = {
        .output_pins = {
            LED_1 | NRFX_PWM_PIN_INVERTED,
            NRFX_PWM_PIN_NOT_USED,
            NRFX_PWM_PIN_NOT_USED,
            NRFX_PWM_PIN_NOT_USED,
        },
        .irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
        .base_clock = (nrf_pwm_clk_t)NRFX_PWM_DEFAULT_CONFIG_BASE_CLOCK,
        .count_mode = (nrf_pwm_mode_t)NRFX_PWM_DEFAULT_CONFIG_COUNT_MODE,
        .top_value = TOP_VALUE,
        .load_mode = (nrf_pwm_dec_load_t)NRF_PWM_LOAD_COMMON,
        .step_mode = (nrf_pwm_dec_step_t)NRFX_PWM_DEFAULT_CONFIG_STEP_MODE,
    };

    APP_ERROR_CHECK(nrfx_pwm_init(&pwm_indicator, &config_indicator, NULL));
}
