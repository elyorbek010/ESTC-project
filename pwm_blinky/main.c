#include "modules/logs.h"
#include "modules/button.h"
#include "modules/hsv_rgb.h"
#include "modules/pwm.h"
#if (ESTC_USB_CLI_ENABLED == 1)
#include "modules/my_cli.h"
#endif
#include "modules/fstorage.h"
#include "modules/my_ble.h"

#include "app_timer.h"
#include "nrf_log.h"
#include "app_usbd.h"
#include "nrf_pwr_mgmt.h"

#define SLEEP_TIME_MS 500

APP_TIMER_DEF(wake_up_timer);
void timeout_wake_up_handler(void *p_context);

void timer_init(void);

int main(void)
{
  color_init();
  logs_init();
  timer_init();
  pwm_init();
  button_init();
#if (ESTC_USB_CLI_ENABLED == 1)
  cli_init();
#endif
  fstorage_init();
  my_ble_init();

  while (true)
  {

// process cli
#if (ESTC_USB_CLI_ENABLED == 1)
    while (app_usbd_event_queue_process())
      ;
    cli_process();
#endif

    // flush all buffered logs
    send_logs();

    nrf_pwr_mgmt_run();
  }
}

void timer_init(void)
{
  ret_code_t ret;
  ret = app_timer_init();
  APP_ERROR_CHECK(ret);

  ret = app_timer_create(&wake_up_timer, APP_TIMER_MODE_REPEATED, timeout_wake_up_handler);
  APP_ERROR_CHECK(ret);

  ret = app_timer_start(wake_up_timer, APP_TIMER_TICKS(SLEEP_TIME_MS), NULL);
  APP_ERROR_CHECK(ret);
}

void timeout_wake_up_handler(void *p_context)
{
  if (is_mode_changed())
  {
    pwm_indicator_update((pwm_indicator_modes_t)get_board_mode());
  }

  if (!is_color_saved())
  {
    uint32_t red = 0, green = 0, blue = 0;
    get_rgb(&red, &green, &blue);

    pwm_rgb_update(red, green, blue);
    ble_rgb_update();

    save_color();

    float h, s, v;
    get_hsv(&h, &s, &v);

    NRF_LOG_INFO("hue: %u\u00B0, saturation: %u%%, value: %u%%",
                 (uint32_t)h, (uint32_t)s, (uint32_t)v);

    NRF_LOG_INFO("red: %u, green: %u, blue: %u\n",
                 red, green, blue);
  }
}

void power_management_init(void)
{
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}