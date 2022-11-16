#include "modules/logs.h"
#include "modules/pwm.h"
#include "modules/button.h"

#define DEVICE_ID 7201
#define CYCLE_PERIOD 1000 // T=1000us = 1ms,  Freq = 1/T = 1kHz

bool state; // State of the device leds
extern uint32_t t_on;
extern uint32_t duty_cycle;
extern bool btn_pressed;
extern bool btn_double_click;

int main(void)
{
  logs_init();
  bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
  timer_init();
  gpiote_init();

  while (true)
  {
    send_log(); // Periodically send logs

    if (btn_double_click)
    {
      state ^= true; // invert state
      btn_double_click = false;

      if (state == true)
      {
        NRF_LOG_INFO("Button is double clicked: state = on");
      }
      else
      {
        NRF_LOG_INFO("Button is double clicked: state = off");
      }
    }

    if (state == true)
    {
      fluctuate_duty_cycle(); // change duty cycle 0-100-0 percent
      pwm_blink(t_on, CYCLE_PERIOD);
    }
  }
}
