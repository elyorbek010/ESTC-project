#include "hsv_rgb.h"

#include "math.h"
#include "nrf_log.h"

#define MAX_RGB_VALUE 255 // red, green, blue each have values up to 255
#define MIN_VALUE 0

typedef enum
{
  DOWN,
  UP
} my_dir_t;

static float slide_speed[3] = {HUE_CHANGE_STEP, SATURATION_CHANGE_STEP, VALUE_CHANGE_STEP};
static float max_hsv_value[3] = {360, 100, 100}; // Max HUE = 360°, Max SATURATION 100%, Max VALUE 100%

struct Color
{
  uint8_t rgb[3];
  float hsv[3];
  my_dir_t dir[3];

  bool saved; // 0 - saved, 1 - saved
} color;

static void hsv2rgb(void)
{
  float r = 0, g = 0, b = 0;

  float h = color.hsv[HUE];
  float s = color.hsv[SATURATION] / 100;
  float v = color.hsv[VALUE] / 100;

  if (s == 0)
  {
    r = g = b = v;
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

  color.rgb[RED] = (uint8_t)(r * MAX_RGB_VALUE);
  color.rgb[GREEN] = (uint8_t)(g * MAX_RGB_VALUE);
  color.rgb[BLUE] = (uint8_t)(b * MAX_RGB_VALUE);
}

static float min(float a, float b)
{
  return a < b ? a : b;
}

static float max(float a, float b)
{
  return a > b ? a : b;
}

static void rgb2hsv(void)
{
  float delta, min_;
  float h = 0, s = 0, v = 0;

  min_ = min(min(color.rgb[RED], color.rgb[GREEN]), color.rgb[BLUE]);
  v = max(max(color.rgb[RED], color.rgb[GREEN]), color.rgb[BLUE]);
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
    if (color.rgb[RED] == v)
    {
      h = ((float)color.rgb[GREEN] - (float)color.rgb[BLUE]) / delta;
    }
    else if (color.rgb[GREEN] == v)
    {
      h = 2 + ((float)color.rgb[BLUE] - (float)color.rgb[RED]) / delta;
    }
    else if (color.rgb[BLUE] == v)
    {
      h = 4 + ((float)color.rgb[RED] - (float)color.rgb[GREEN]) / delta;
    }
    h *= 60;
    if (h < 0)
    {
      h += 360;
    }
  }

  color.hsv[HUE] = h;
  color.hsv[SATURATION] = s * 100;
  color.hsv[VALUE] = v / 255 * 100;
}

void set_rgb(uint32_t red, uint32_t green, uint32_t blue)
{
  if (red > 255)
    red = MAX_RGB_VALUE;
  if (green > 255)
    red = MAX_RGB_VALUE;
  if (blue > 255)
    red = MAX_RGB_VALUE;

  color.rgb[RED] = (uint8_t)red;
  color.rgb[GREEN] = (uint8_t)green;
  color.rgb[BLUE] = (uint8_t)blue;

  rgb2hsv();

  color.saved = false;
}

void get_rgb(uint32_t *red, uint32_t *green, uint32_t *blue)
{
  *red = color.rgb[RED];
  *green = color.rgb[GREEN];
  *blue = color.rgb[BLUE];
}

void set_hsv(float hue, float saturation, float value)
{
  if (hue > max_hsv_value[HUE])
    hue = max_hsv_value[HUE];
  if (saturation > max_hsv_value[SATURATION])
    saturation = max_hsv_value[SATURATION];
  if (value > max_hsv_value[VALUE])
    value = max_hsv_value[VALUE];

  if (hue < MIN_VALUE)
    hue = MIN_VALUE;
  if (saturation < MIN_VALUE)
    saturation = MIN_VALUE;
  if (value < MIN_VALUE)
    value = MIN_VALUE;

  color.hsv[HUE] = hue;
  color.hsv[SATURATION] = saturation;
  color.hsv[VALUE] = value;

  hsv2rgb();

  color.saved = false;
}

void get_hsv(float *hue, float *saturation, float *value)
{
  *hue = color.hsv[HUE];
  *saturation = color.hsv[SATURATION];
  *value = color.hsv[VALUE];
}

void slide_hsv(hsv_t option)
{
  color.hsv[option] = (color.dir[option] == UP) ? (color.hsv[option] + slide_speed[option]) : (color.hsv[option] - slide_speed[option]);
  if (color.hsv[option] >= max_hsv_value[option])
  {
    color.dir[option] = DOWN;
    color.hsv[option] = max_hsv_value[option];
  }
  else if (color.hsv[option] <= MIN_VALUE)
  {
    color.dir[option] = UP;
    color.hsv[option] = MIN_VALUE;
  }
  hsv2rgb();
  color.saved = false;
}

bool is_color_saved(void)
{
  return color.saved;
}

void set_color_saved(void)
{
  color.saved = true;
}

void color_init(void)
{
  color.rgb[RED] = MAX_RGB_VALUE;
  color.rgb[GREEN] = MAX_RGB_VALUE;
  color.rgb[BLUE] = MAX_RGB_VALUE;

  rgb2hsv();

  color.dir[HUE] = UP;
  color.dir[SATURATION] = UP;
  color.dir[VALUE] = UP;

  color.saved = false;
}