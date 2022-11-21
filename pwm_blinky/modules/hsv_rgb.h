#ifndef HSV_RGB_H
#define HSV_RGB_H

#include "math.h"
#include "logs.h"
#include "pwm.h"

void hsv2rgb(float, float, float, uint32_t *, uint32_t *, uint32_t *);

void no_modif(void);

void hue_modif(float *);

void satur_modif(float *);

void val_modif(float *);

#endif // HSV_RGB_H