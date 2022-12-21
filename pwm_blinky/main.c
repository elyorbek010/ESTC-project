#include "modules/logs.h"
#include "modules/button.h"
#include "modules/hsv_rgb.h"
#include "modules/pwm.h"
#include "modules/nvmc.h"
#include "modules/my_cli.h"

#define DEVICE_ID 7201
#define SLEEP_TIME 30

APP_TIMER_DEF(wake_up_timer);
void timeout_wake_up_handler(void *p_context);

static const uint32_t MODES_N = 4;
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
  timer_init();
  gpiote_init();
  pwm_init();
  cli_init();

  app_timer_create(&wake_up_timer, APP_TIMER_MODE_REPEATED, timeout_wake_up_handler);
  app_timer_start(wake_up_timer, APP_TIMER_TICKS(SLEEP_TIME), NULL);

  my_modes_t mode = NO_INPUT;
  Color color = {0, 0, 0, 3.6, 1, 1, 0, 0};

  nvmc_init(&color);

  while (true)
  {
    send_log();
    // NRF_LOG_INFO("%u, %u", cur_list_addr, n_elem);
    // NRF_LOG_INFO("%s", colors_list[0].name);

    // char *buffer = (char*)cur_list_addr;
    // NRF_LOG_PUSH(buffer);
    // NRF_LOG_INFO("size of buffer = %d", sizeof(buffer));
    // change mode when button is double clicked
    if (is_double_clicked())
    {
      mode++;
      if (mode >= MODES_N)
      {
        mode = NO_INPUT;
      }
      pwm_indicator_update(mode);
    }

    // increment hsv value if holding button pressed
    if (is_pressed())
    {
      switch (mode)
      {
      case NO_INPUT:
        // no_input();
        break;
      case HUE_MODIFY:
        hue_modif(&color);
        break;
      case SATURATION_MODIFY:
        satur_modif(&color);
        break;
      case VALUE_MODIFY:
        val_modif(&color);
        break;
      default:
        break;
      }
    }

    // process cli commands
    cli_process(&color);

    if (color.modified)
    {
      pwm_rgb_update(color.red, color.green, color.blue);
      NRF_LOG_INFO("hue: %u, saturation: %u, value: %u\nred: %u, green: %u, blue: %u\n",
                   (uint32_t)color.hue, (uint32_t)(color.saturation * 100), (uint32_t)(color.value * 100),
                   color.red, color.green, color.blue);
    }

    if (!is_pressed())
    {
      if (color.modified)
      {
        NRF_LOG_INFO("SAVED");
        write_color(&color);
      }
      __WFI();
    }
  }
}

void timeout_wake_up_handler(void *p_context)
{
  // NRF_LOG_INFO("wake up");
}