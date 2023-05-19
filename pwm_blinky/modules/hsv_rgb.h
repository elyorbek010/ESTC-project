#ifndef HSV_RGB_H
#define HSV_RGB_H

#include "stdbool.h"
#include "stdint.h"

#define HUE_CHANGE_STEP 8
#define SATURATION_CHANGE_STEP 4
#define VALUE_CHANGE_STEP 4

typedef enum
{
    RED,
    GREEN,
    BLUE
} rgb_t;

typedef enum
{
    HUE,
    SATURATION,
    VALUE
} hsv_t;

void color_init(void);

void set_rgb(uint32_t red, uint32_t green, uint32_t blue);

void get_rgb(uint32_t *red, uint32_t *green, uint32_t *blue); // MAX_RGB_VALUE = 255

void set_hsv(float hue, float saturation, float value);

void get_hsv(float *hue, float *saturation, float *value); // Max HUE = 360°, Max SATURATION 100%, Max VALUE 100%

// slide hsv values specified by option from 0% to 100% back and forth
void slide_hsv(hsv_t option);

bool is_color_saved(void);

void set_color_saved(void);

#endif // HSV_RGB_H