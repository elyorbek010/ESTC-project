#include "modules/logs.h"
#include "modules/pwm.h"
#include "modules/button.h"
#include "modules/hsv_rgb.h"

#define DEVICE_ID 7201
#define ID_HUE 4 // 360 * 0.01 = 3.6 ~ 4
#define MODES_N 4

extern bool btn_pressed;
extern bool btn_double_click;

APP_TIMER_DEF(wake_up_timer);
void timeout_wakeup(void *p_context);

int main(void)
{
  logs_init();
  bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
  timer_init();
  gpiote_init();
  pwm_init();

  APP_ERROR_CHECK(app_timer_create(&wake_up_timer, APP_TIMER_MODE_REPEATED, timeout_wakeup));
  app_timer_start(wake_up_timer, APP_TIMER_TICKS(30), NULL);

  uint8_t mode = 1;

  while (true)
  {

    send_log(); // Periodically send logs
    if (btn_double_click)
    {
      mode++;
      btn_double_click = false;
      if (mode > MODES_N)
      {
        mode = 1; // reset mode
      }
      NRF_LOG_INFO("Button is double clicked: mode = %u", mode);
      pwm1(mode); // change mode indicator
    }

    while (btn_pressed)
    {
      switch (mode)
      {
      case 1:
        no_modif();
        break;
      case 2:
        hue_modif();
        break;
      case 3:
        satur_modif();
        break;
      case 4:
        value_modif();
        break;
      default:
        continue;
        break;
      }
    }
    __WFI();
  }
}

void timeout_wakeup(void *p_context)
{
  hsv2rgb();
}