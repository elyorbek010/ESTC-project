#include "hsv_rgb.h"

static float hue = 4; // 360 * 0.01 = 3.6 ~ 4
static float saturation = 1;
static float value = 1;

void hsv2rgb(void)
{
  float c = saturation * value;
  float x = c * (1 - fabs(hue / 60 - ((int)((hue / 60) / 2)) * 2 - 1));
  float m = value - c;
  float r, g, b;
  if (hue >= 0 && hue < 60)
  {
    r = c;
    g = x;
    b = 0;
  }
  else if (hue >= 60 && hue < 120)
  {
    r = x;
    g = c;
    b = 0;
  }
  else if (hue >= 120 && hue < 180)
  {
    r = 0;
    g = c;
    b = x;
  }
  else if (hue >= 180 && hue < 240)
  {
    r = 0;
    g = x;
    b = c;
  }
  else if (hue >= 240 && hue < 300)
  {
    r = x;
    g = 0;
    b = c;
  }
  else if (hue >= 300 && hue < 360)
  {
    r = c;
    g = 0;
    b = x;
  }

  pwm0(((uint16_t)((r + m) * 255)), ((uint16_t)((g + m) * 255)), ((uint16_t)((b + m) * 255)));
}

void no_modif(void)
{
  bsp_board_leds_off();
  NRF_LOG_INFO("No modify: leds off");
}

void hue_modif(void)
{
  hue += 0.01;
  if (hue >= 360)
  {
    hue = 0;
  }
  NRF_LOG_INFO("Hue modify: hue = %u", hue);
}

void satur_modif(void)
{
  saturation += 0.00005;
  if (saturation > 1)
  {
    saturation = 0;
  }
  NRF_LOG_INFO("Saturation modify: saturation = %u%%", (uint8_t)((saturation) * 100));
}

void value_modif(void)
{
  value += 0.00005;
  if (value > 1)
  {
    value = 0;
  }
  NRF_LOG_INFO("Value modify: value = %u%%", (uint8_t)((value) * 100));
}