#ifndef MY_CLI_H
#define MY_CLI_H

#include <stdbool.h>
#include <stdint.h>
#include "logs.h"
#include "app_usbd_cdc_acm.h"
#include "hsv_rgb.h"
#include "nvmc.h"

#define M_COMMANDS 8

typedef enum
{
    help,
    RGB,
    HSV,
    ADD_RGB_COLOR,
    ADD_CURRENT_COLOR,
    DEL_COLOR,
    APPLY_COLOR,
    LIST_COLORS
} my_command_t;

static const char commands[M_COMMANDS][20] = {"help", "RGB", "HSV", "add_rgb_color",
                                              "add_current_color", "del_color", "apply_color", "list_colors"};

void cli_process(Color *color);

void cli_init(void);

#endif // MY_CLI_H