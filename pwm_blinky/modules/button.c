#include "button.h"

static const uint32_t DEBOUNCE_DELAY = 50;
static const uint32_t BUTTON_EFFECT_DELAY = 500;

APP_TIMER_DEF(timer_debounce);
APP_TIMER_DEF(timer_btn_effect);

my_button_t button = {
    BUTTON_1,
    0,
    0,
    RELEASED,
    IDLE};

bool is_clicked(void)
{
    if (button.status == CLICKED)
    {
        button.status = IDLE;
        return true;
    }
    else
    {
        return false;
    }
}

bool is_pressed(void)
{
    return (button.status == PRESSED);
}

bool is_double_clicked(void)
{
    if (button.status == DOUBLE_CLICKED)
    {
        button.status = IDLE;
        return true;
    }
    else
    {
        return false;
    }
}

void timeout_debounce_handler(void *p_context)
{
    if (button.status == CLICKED || button.status == DOUBLE_CLICKED)
    { // if status was set but was not used then reset it
        button.status = IDLE;
        //NRF_LOG_INFO("CLICKED || DOUBLE CLICKED unused - > IDLE");
    }

    if (button.status == IDLE && button.state == RELEASED && bsp_board_button_state_get(button.idx) == true)
    {
        button.state = PUSHED;
        //NRF_LOG_INFO("PUSHED");
    }
    else if (button.status == IDLE && button.state == PUSHED && bsp_board_button_state_get(button.idx) == false)
    {
        button.state = RELEASED;
        button.clicks_n++;
        //NRF_LOG_INFO("RELEASED");
    }
    else if (button.status == PRESSED && button.state == PUSHED && bsp_board_button_state_get(button.idx) == false)
    {
        button.status = IDLE;
        button.state = RELEASED;
        //NRF_LOG_INFO("RELEASED FROM LONG PRESS");
    }
    app_timer_start(timer_btn_effect, APP_TIMER_TICKS(BUTTON_EFFECT_DELAY), NULL);
}

void timeout_btn_effect_handler(void *p_context)
{
    if (button.status == IDLE && button.state == PUSHED && bsp_board_button_state_get(button.idx) == true)
    {
        button.status = PRESSED;
        //NRF_LOG_INFO("PRESSED");
    }
    else if (button.clicks_n == 1)
    {
        button.status = CLICKED;
        button.clicks_n = 0;
        //NRF_LOG_INFO("CLICKED");
    }
    else if (button.clicks_n > 1)
    {
        button.status = DOUBLE_CLICKED;
        button.clicks_n = 0;
        //NRF_LOG_INFO("DOUBLE_CLICKED");
    }
}

void gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    app_timer_start(timer_debounce, APP_TIMER_TICKS(DEBOUNCE_DELAY), NULL);
}

void timer_init(void)
{
    // nrfx_systick_init();

    ret_code_t ret;
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&timer_debounce, APP_TIMER_MODE_SINGLE_SHOT, timeout_debounce_handler);
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&timer_btn_effect, APP_TIMER_MODE_SINGLE_SHOT, timeout_btn_effect_handler);
    APP_ERROR_CHECK(ret);
}

void gpiote_init(void)
{
    ret_code_t ret;
    ret = nrfx_gpiote_init();
    APP_ERROR_CHECK(ret);

    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    ret = nrfx_gpiote_in_init(BUTTON_1, &in_config, gpiote_evt_handler);
    APP_ERROR_CHECK(ret);

    nrfx_gpiote_in_event_enable(BUTTON_1, true);
}