#include "hsv_rgb.h"

typedef enum
{
  DOWN,
  UP
} my_dir_t;

static my_dir_t hue_dir;
static my_dir_t satur_dir;
static my_dir_t val_dir;

void no_input(void)
{
}

void hue_modif(Color *color)
{
  color->hue = hue_dir ? (color->hue + HUE_CHANGE_STEP) : (color->hue - HUE_CHANGE_STEP);
  if (color->hue > 360.0)
  {
    hue_dir = DOWN;
    color->hue = 360.0;
  }
  else if (color->hue < 0)
  {
    hue_dir = UP;
    color->hue = 0;
  }
  hsv2rgb(color);
  color->modified = true;
  // NRF_LOG_INFO("hue_modif: %u", (uint32_t)color->hue);
}

void satur_modif(Color *color)
{
  color->saturation = satur_dir ? (color->saturation + SATURATION_CHANGE_STEP) : (color->saturation - SATURATION_CHANGE_STEP);
  if (color->saturation > 1.0)
  {
    satur_dir = DOWN;
    color->saturation = 1.0;
  }
  else if (color->saturation < 0)
  {
    satur_dir = UP;
    color->saturation = 0;
  }
  hsv2rgb(color);
  color->modified = true;
  // NRF_LOG_INFO("satur_modif: %u", (uint32_t)(color->saturation * 100));
}

void val_modif(Color *color)
{
  color->value = val_dir ? (color->value + VALUE_CHANGE_STEP) : (color->value - VALUE_CHANGE_STEP);
  if (color->value > 1.0)
  {
    val_dir = DOWN;
    color->value = 1.0;
  }
  else if (color->value < 0)
  {
    val_dir = UP;
    color->value = 0;
  }
  hsv2rgb(color);
  color->modified = true;
  // NRF_LOG_INFO("val_modif: %u", (uint32_t)(color->value * 100));
}

void hsv2rgb(Color *color)
{
  float r = 0, g = 0, b = 0;
  float h = color->hue;
  float s = color->saturation;
  float v = color->value;

  if (s == 0)
  {
    r = v;
    g = v;
    b = v;
  }
  else
  {
    int i;
    float f, p, q, t;

    if (h == 360)
    {
      h = 0;
    }
    else
    {
      h /= 60;
    }

    i = (int)trunc(h);
    f = h - i;

    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    switch (i)
    {
    case 0:
      r = v;
      g = t;
      b = p;
      break;

    case 1:
      r = q;
      g = v;
      b = p;
      break;

    case 2:
      r = p;
      g = v;
      b = t;
      break;

    case 3:
      r = p;
      g = q;
      b = v;
      break;

    case 4:
      r = t;
      g = p;
      b = v;
      break;

    default:
      r = v;
      g = p;
      b = q;
      break;
    }
  }

  color->red = (uint32_t)(r * 255);
  color->green = (uint32_t)(g * 255);
  color->blue = (uint32_t)(b * 255);
}

static float min(float a, float b)
{
  return a <= b ? a : b;
}

static float max(float a, float b)
{
  return a >= b ? a : b;
}

void rgb2hsv(Color *color)
{
  float delta, min_;
  float h, s, v;

  min_ = min(min(color->red, color->green), color->blue);
  v = max(max(color->red, color->green), color->blue);
  delta = v - min_;

  if (v == 0)
  {
    s = 0;
  }
  else
  {
    s = delta / v;
  }

  if (s == 0)
  {
    h = 0;
  }
  else
  {
    if (color->red == v)
    {
      h = ((float)color->green - (float)color->blue) / delta;
    }
    else if (color->green == v)
    {
      h = 2 + ((float)color->blue - (float)color->red) / delta;
    }
    else if (color->blue == v)
    {
      h = 4 + ((float)color->red - (float)color->green) / delta;
    }
    h *= 60;
    if (h < 0)
    {
      h += 360;
    }
  }

  color->hue = h;
  color->saturation = s;
  color->value = v / 255;
  NRF_LOG_INFO("RGB TO HSV:\nhue: %u, saturation: %u, value: %u\nred: %u, green: %u, blue: %u\n",
               (uint32_t)color->hue, (uint32_t)(color->saturation * 100), (uint32_t)(color->value * 100),
               color->red, color->green, color->blue);
}
