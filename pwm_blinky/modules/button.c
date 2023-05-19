#include "button.h"
#include "hsv_rgb.h"

#include "nrfx_gpiote.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_log.h"
#include "stdint.h"
#include "boards.h"

#define BUTTON NRF_GPIO_PIN_MAP(BUTTON_PORT, BUTTON_PIN)

#define DEBOUNCE_DELAY 50
#define BUTTON_EFFECT_DELAY 500

APP_TIMER_DEF(timer_debounce);
APP_TIMER_DEF(timer_btn_long_press_effect);

APP_TIMER_DEF(timer_btn_dbl_clk_timeout);

void btn_dbl_clk_effect_handler(void *p_context);

static board_modes_t current_mode;
static bool mode_changed; // if changed - 1, if checked - 0

typedef enum
{
    RELEASED,
    PUSHED
} my_button_state_t;

typedef enum
{
    IDLE,
    CLICK,
    LONG_PRESS,
    DOUBLE_CLICK
} my_button_status_t;

typedef struct
{
    nrfx_gpiote_pin_t pin;
    uint8_t idx;
    uint8_t clicks_n;
    my_button_state_t state;
    my_button_status_t status;
} my_button_t;

my_button_t button = {
    .pin = BUTTON,
    .idx = 0,
    .clicks_n = 0,
    .state = RELEASED,
    .status = IDLE};

void next_mode(void)
{
    switch (current_mode)
    {
    case NO_INPUT:
        current_mode = HUE_MODIFY;
        break;
    case HUE_MODIFY:
        current_mode = SATURATION_MODIFY;
        break;
    case SATURATION_MODIFY:
        current_mode = VALUE_MODIFY;
        break;
    case VALUE_MODIFY:
        current_mode = NO_INPUT;
        break;
    default:
        current_mode = NO_INPUT;
        break;
    }

    mode_changed = true;
}

board_modes_t get_board_mode(void)
{
    return current_mode;
}

bool is_mode_changed(void)
{
    if (mode_changed)
    {
        mode_changed = false;
        return true;
    }
    else
    {
        return false;
    }
}

void timeout_debounce_handler(void *p_context)
{
    if (button.status == CLICK || button.status == DOUBLE_CLICK)
    { // if status was set but was not used, reset it
        button.status = IDLE;
        // NRF_LOG_INFO("CLICK || DOUBLE CLICK unused - > IDLE");
    }

    if (button.status == IDLE && button.state == RELEASED && bsp_board_button_state_get(button.idx) == true)
    {
        button.state = PUSHED;
        app_timer_start(timer_btn_long_press_effect, APP_TIMER_TICKS(BUTTON_EFFECT_DELAY), NULL);
        // NRF_LOG_INFO("PUSHED");
    }
    else if (button.status == IDLE && button.state == PUSHED && bsp_board_button_state_get(button.idx) == false)
    {
        // NRF_LOG_INFO("RELEASED");
        button.state = RELEASED;

        btn_dbl_clk_effect_handler(NULL);

        app_timer_stop(timer_btn_long_press_effect);
    }
    else if (button.status == LONG_PRESS && button.state == PUSHED && bsp_board_button_state_get(button.idx) == false)
    {
        app_timer_stop(timer_btn_long_press_effect);
        button.status = IDLE;
        button.state = RELEASED;
        // NRF_LOG_INFO("RELEASED FROM LONG PRESS");
    }
    else
    {
        // NRF_LOG_INFO("ERROR: BUTTON");
        button.clicks_n = 0;
        button.state = RELEASED;
        button.status = IDLE;
    }
}

void timeout_btn_long_press_effect_handler(void *p_context)
{
    button.status = LONG_PRESS;
    switch (current_mode)
    {
    case NO_INPUT:
        break;
    case HUE_MODIFY:
        slide_hsv(HUE);
        break;
    case SATURATION_MODIFY:
        slide_hsv(SATURATION);
        break;
    case VALUE_MODIFY:
        slide_hsv(VALUE);
        break;
    }
    // NRF_LOG_INFO("LONG_PRESS");
}

void timeout_btn_dbl_clk_timeout(void *p_context)
{
    // NRF_LOG_INFO("Double Click Timed Out");
    button.clicks_n = 0;
}

void btn_dbl_clk_effect_handler(void *p_context)
{
    if (button.clicks_n == 1)
    {
        app_timer_stop(timer_btn_dbl_clk_timeout);
        button.status = DOUBLE_CLICK;
        button.clicks_n = 0;
        next_mode();
        // NRF_LOG_INFO("DOUBLE_CLICK");
    }
    else
    {
        app_timer_start(timer_btn_dbl_clk_timeout, APP_TIMER_TICKS(BUTTON_EFFECT_DELAY), NULL);
        button.status = CLICK;
        button.clicks_n = 1;
        // NRF_LOG_INFO("CLICK");
    }
}

void gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    app_timer_start(timer_debounce, APP_TIMER_TICKS(DEBOUNCE_DELAY), NULL);
}

void timer_create(void)
{
    ret_code_t ret = app_timer_create(&timer_debounce, APP_TIMER_MODE_SINGLE_SHOT, timeout_debounce_handler);
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&timer_btn_long_press_effect, APP_TIMER_MODE_REPEATED, timeout_btn_long_press_effect_handler);
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&timer_btn_dbl_clk_timeout, APP_TIMER_MODE_SINGLE_SHOT, timeout_btn_dbl_clk_timeout);
    APP_ERROR_CHECK(ret);
}

void gpiote_init(void)
{
    ret_code_t ret;
    ret = nrfx_gpiote_init();
    APP_ERROR_CHECK(ret);

    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    ret = nrfx_gpiote_in_init(button.pin, &in_config, gpiote_evt_handler);
    APP_ERROR_CHECK(ret);

    nrfx_gpiote_in_event_enable(button.pin, true);
}

void button_init()
{
    timer_create();
    gpiote_init();
    current_mode = NO_INPUT;
    mode_changed = true;
}