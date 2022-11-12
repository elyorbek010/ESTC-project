#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nordic_common.h"
#include "boards.h"
#include "app_timer.h"
#include "drv_rtc.h"
#include "nrfx_gpiote.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrfx_systick.h"
#include "pca10059.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "app_usbd.h"
#include "app_usbd_serial_num.h"

uint8_t m_board_led_list[LEDS_NUMBER] = LEDS_LIST;
APP_TIMER_DEF(timer0);
APP_TIMER_DEF(timer1);

uint32_t t_on;
uint32_t t_period = 1000; // T=1000us = 1ms,  Freq = 1/T = 1kHz
uint16_t led_idx = -1;
bool state_on = true;
bool up_flag = 1;
bool btn_pressed;
uint16_t btn_presses_n;

static void timeout_handler_0(void *p_context)
{
  if (bsp_board_button_state_get(BSP_BOARD_BUTTON_0) == true)
  {
    btn_pressed = true;
    btn_presses_n++;
    app_timer_start(timer1, APP_TIMER_TICKS(500), NULL);
  }
  else
  {
    btn_pressed = false;
  }
}

static void timeout_handler_1(void *p_context)
{
  if (btn_presses_n >= 2)
  {
    state_on ^= true;
  }
  btn_presses_n = 0;
}

void timer_init(void)
{
  /*
  // initialize the low power low frequency clock
  ret_code_t err_code = nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);

  // request the lf clock to not to generate any events on ticks
  // One tick =  1 value increment in the counter register
  nrf_drv_clock_lfclk_request(NULL);
  */

  app_timer_init();
  app_timer_create(&timer0, APP_TIMER_MODE_SINGLE_SHOT, timeout_handler_0);
  app_timer_create(&timer1, APP_TIMER_MODE_SINGLE_SHOT, timeout_handler_1);
}

void gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  app_timer_start(timer0, APP_TIMER_TICKS(50), NULL);
}

void gpiote_init(void)
{

  nrfx_gpiote_init();

  nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
  in_config.pull = NRF_GPIO_PIN_PULLUP;
  nrfx_gpiote_in_init(BUTTON_1, &in_config, gpiote_evt_handler);
  nrfx_gpiote_in_event_enable(BUTTON_1, true);
}

void pwm_blink(uint32_t t_on_, uint32_t t_period_)
{
  uint32_t t_off_ = t_period_ - t_on_;

  nrfx_systick_state_t time_cap;
  nrfx_systick_get(&time_cap);

  while (nrfx_systick_test(&time_cap, t_on_) == false)
  {
    bsp_board_led_on(led_idx);
  }

  nrfx_systick_get(&time_cap);
  
  while (nrfx_systick_test(&time_cap, t_off_) == false)
  {
    bsp_board_led_off(led_idx);
  }
}

void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

int main(void)
{
  logs_init();

  NRF_LOG_INFO("Starting up the test project with USB logging");

  bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

  timer_init();
  nrfx_systick_init();
  gpiote_init();

  int iter_count = 0;
  while (true)
  {
    NRF_LOG_INFO("Iter %d", iter_count++);

    LOG_BACKEND_USB_PROCESS();
    NRF_LOG_PROCESS();

    if(state_on == false){
      continue;
    }

    if (btn_pressed)
    {
      if (t_on == 1000)
      {
        up_flag = false;
      }
      else if (t_on == 0)
      {
        up_flag = true;
        led_idx++;
        if (led_idx == LEDS_NUMBER)
        {
          led_idx = 0;
        }
      }

      if (up_flag)
      {
        t_on++;
      }
      else
      {
        t_on--;
      }
    }
    pwm_blink(t_on, t_period);
  }
}
