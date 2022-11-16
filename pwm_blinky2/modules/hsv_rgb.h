#ifndef HSV_RGB_H
#define HSV_RGB_H

#include "math.h"
#include "logs.h"
#include "pwm.h"

void hsv2rgb(float hue, float saturation, float value);

void no_modif(void);

void hue_modif(float *hue);

void satur_modif(float *saturation);

void value_modif(float *value);

#endif // HSV_RGB_H