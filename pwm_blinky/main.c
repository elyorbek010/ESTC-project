#include "modules/logs.h"
#include "modules/button.h"
#include "modules/hsv_rgb.h"
#include "modules/pwm.h"

#define DEVICE_ID 7201

APP_TIMER_DEF(wake_up);
void timeout_wake_up_handler(void *p_context);

static const uint8_t MODES_N = 4;
typedef enum
{
  NO_MODIFY,
  HUE_MODIFY,
  SATURATION_MODIFY,
  VALUE_MODIFY
} my_modes_t;

int main(void)
{
  logs_init();
  bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
  timer_init();
  gpiote_init();
  pwm_init();

  app_timer_create(&wake_up, APP_TIMER_MODE_REPEATED, timeout_wake_up_handler);
  app_timer_start(wake_up, APP_TIMER_TICKS(50), NULL);

  float hue = 4;        // 360 * 0.01 = 3.6 ~ 4
  float saturation = 1; // 100%
  float value = 1;      // 100%

  uint32_t red;
  uint32_t green;
  uint32_t blue;

  my_modes_t mode = NO_MODIFY;

  hsv2rgb(hue, saturation, value, &red, &green, &blue);
  pwm_indicator_update(mode);
  pwm_rgb_update(red, green, blue);

  while (true)
  {
    send_log(); // Periodically send logs

    if (is_double_clicked()) // change mode when button double clicked
    {
      mode++;
      if (mode >= MODES_N)
      {
        mode = NO_MODIFY;
      }
      pwm_indicator_update(mode);
    }

    while (is_pressed()) // change hsv values when holding button pressed
    {
      switch (mode)
      {
      case NO_MODIFY:
        no_modif();
        break;
      case HUE_MODIFY:
        hue_modif(&hue);
        break;
      case SATURATION_MODIFY:
        satur_modif(&saturation);
        break;
      case VALUE_MODIFY:
        val_modif(&value);
        break;
      default:
        break;
      }
      hsv2rgb(hue, saturation, value, &red, &green, &blue);
      pwm_rgb_update(red, green, blue);
    }
    __WFI();
  }
}

void timeout_wake_up_handler(void *p_context)
{
}