#include "button.h"

APP_TIMER_DEF(timer_debounce);
APP_TIMER_DEF(timer_dbl_clck);

void timeout_debounce_handler(void *p_context)
{
    if (btn_pressed(BSP_BOARD_BUTTON_0))
    {
        btn_pressed = true;
        btn_presses_n++;
        app_timer_start(timer_dbl_clck, APP_TIMER_TICKS(DOUBLE_CLICK_DELAY), NULL);
    }
    else
    {
        btn_pressed = false;
    }
}

void timeout_dbl_clck_handler(void *p_context)
{
    btn_double_click = (btn_presses_n >= 2) ? true : false;
    btn_presses_n = 0;
}

void gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    app_timer_start(timer_debounce, APP_TIMER_TICKS(DEBOUNCE_DELAY), NULL);
}

void timer_init(void)
{
    //nrfx_systick_init();

    ret_code_t ret;
    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&timer_debounce, APP_TIMER_MODE_SINGLE_SHOT, timeout_debounce_handler);
    APP_ERROR_CHECK(ret);

    ret = app_timer_create(&timer_dbl_clck, APP_TIMER_MODE_SINGLE_SHOT, timeout_dbl_clck_handler);
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