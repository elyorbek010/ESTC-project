#include "hsv_rgb.h"

static const float HUE_CHANGE_STEP = 0.01;
static const float SATURATION_CHANGE_STEP = 0.00002;
static const float VALUE_CHANGE_STEP = 0.00002;

static bool hue_dir;   // 1 - up, 0 - down
static bool satur_dir; // 1 - up, 0 - down
static bool val_dir;   // 1 - up, 0 - down

void hsv2rgb(float hue, float saturation, float value, uint32_t *red, uint32_t *green, uint32_t *blue)
{
  float c = saturation * value;
  float x = c * (1 - fabs(hue / 60 - ((int)((hue / 60) / 2)) * 2 - 1));
  float m = value - c;

  if (hue >= 0 && hue < 60)
  {
    *red = (c + m) * 255;
    *green = (x + m) * 255;
    *blue = m * 255;
  }
  else if (hue >= 60 && hue < 120)
  {
    *red = (x + m) * 255;
    *green = (c + m) * 255;
    *blue = m * 255;
  }
  else if (hue >= 120 && hue < 180)
  {
    *red = m * 255;
    *green = (c + m) * 255;
    *blue = (x + m) * 255;
  }
  else if (hue >= 180 && hue < 240)
  {
    *red = m * 255;
    *green = (x + m) * 255;
    *blue = (c + m) * 255;
  }
  else if (hue >= 240 && hue < 300)
  {
    *red = (x + m) * 255;
    *green = m * 255;
    *blue = (c + m) * 255;
  }
  else if (hue >= 300 && hue < 360)
  {
    *red = (c + m) * 255;
    *green = m * 255;
    *blue = (x + m) * 255;
  }
}

void no_input(void)
{
  // NRF_LOG_INFO("no modif");
}

void hue_modif(float * hue)
{
  *hue = hue_dir ? (*hue + HUE_CHANGE_STEP) : (*hue - HUE_CHANGE_STEP);
  if(*hue >= 360){
    hue_dir = 0;
    *hue = 360;
  } else if(*hue <= 0){
    hue_dir = 1;
    *hue = 0;
  }
  // NRF_LOG_INFO("hue modify: hue = %u", (uint32_t)(*hue));
}

void satur_modif(float * saturation)
{
  *saturation = satur_dir ? (*saturation + SATURATION_CHANGE_STEP) : (*saturation - SATURATION_CHANGE_STEP);
  if(*saturation >= 1){
    satur_dir = 0;
    *saturation = 1;
  } else if(*saturation <= 0){
    satur_dir = 1;
    *saturation = 0;
  }
  // NRF_LOG_INFO("saturation modify: saturation = %u%%", (uint32_t)((*saturation) * 100));
}

void val_modif(float * value)
{
  *value = val_dir ? (*value + VALUE_CHANGE_STEP) : (*value - VALUE_CHANGE_STEP);
  if(*value >= 1){
    val_dir = 0;
    *value = 1;
  } else if(*value <= 0){
    val_dir = 1;
    *value = 0;
  }
  // NRF_LOG_INFO("value modify: value = %u%%", (uint32_t)((*value) * 100));
}