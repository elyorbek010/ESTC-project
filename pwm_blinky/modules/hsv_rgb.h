#ifndef HSV_RGB_H
#define HSV_RGB_H

#include "math.h"
#include "logs.h"

static const float HUE_CHANGE_STEP = 0.02;
static const float SATURATION_CHANGE_STEP = 0.00005;
static const float VALUE_CHANGE_STEP = 0.00005;

typedef struct
{
    uint32_t red;   // 0 to 255
    uint32_t green; // 0 to 255
    uint32_t blue;  // 0 to 255

    float hue;        // 0 to 360
    float saturation; // 0 to 1
    float value;      // 0 to 1

    bool modified; // 0 - saved, 1 - modified

    uint32_t cur_addr;  // current address of rgb values saved in flash memory
} Color;

// Update RGB values given HSV values
void hsv2rgb(Color *);

// Update HSV values given RGB values
void rgb2hsv(Color *);

// Do nothing
void no_input(void);

// Fluctuate hue 0-360-0 with steps of HUE_CHANGE_STEP
void hue_modif(Color *);

// Fluctuate saturation 0-1-0 with steps of SATURATION_CHANGE_STEP
void satur_modif(Color *);

// Fluctuate value 0-1-0 with steps of VALUE_CHANGE_STEP
void val_modif(Color *);

#endif // HSV_RGB_H