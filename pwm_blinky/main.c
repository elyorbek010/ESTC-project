#include "modules/logs.h"
#include "modules/button.h"
#include "modules/hsv_rgb.h"
#include "modules/pwm.h"
#include "modules/nvmc.h"

#define DEVICE_ID 7201
#define SLEEP_TIME 30

bool data_is_modified;

APP_TIMER_DEF(wake_up_timer);
void timeout_wake_up_handler(void *p_context);

static const uint8_t MODES_N = 4;
typedef enum
{
  NO_INPUT,
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

  app_timer_create(&wake_up_timer, APP_TIMER_MODE_REPEATED, timeout_wake_up_handler);
  app_timer_start(wake_up_timer, APP_TIMER_TICKS(SLEEP_TIME), NULL);

  float value;
  float saturation;
  float hue;

  if (nvmc_init())
  {
    value = ((float)pop_from_flash()) / 100;      // turn percent into float value < 1.0
    saturation = ((float)pop_from_flash()) / 100; // turn percent into float value < 1.0
    hue = (float)pop_from_flash();

    push_to_flash((uint32_t)(hue));
    push_to_flash((uint32_t)(saturation * 100)); // turn into percent value
    push_to_flash((uint32_t)(value * 100));      // turn into percent value
  }
  else
  {
    hue = 4;        // 360 * 0.01 = 3.6 ~ 4
    saturation = 1; // 100%
    value = 1;      // 100%
  }

  uint32_t red;
  uint32_t green;
  uint32_t blue;

  my_modes_t mode = NO_INPUT;

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
        mode = NO_INPUT;
      }
      pwm_indicator_update(mode);
    }

    while (is_pressed()) // change hsv values when holding button pressed
    {
      switch (mode)
      {
      case NO_INPUT:
        // no_input();
        if (data_is_modified)
        {
          NRF_LOG_INFO("writing to flash");
          push_to_flash((uint32_t)(hue));
          push_to_flash((uint32_t)(saturation * 100)); // turn into percent value
          push_to_flash((uint32_t)(value * 100));      // turn into percent value
          data_is_modified = false;
        }
        break;
      case HUE_MODIFY:
        hue_modif(&hue);
        data_is_modified = true;
        break;
      case SATURATION_MODIFY:
        satur_modif(&saturation);
        data_is_modified = true;
        break;
      case VALUE_MODIFY:
        val_modif(&value);
        data_is_modified = true;
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
  // NRF_LOG_INFO("wake up");
}