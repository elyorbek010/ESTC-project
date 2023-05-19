#ifndef BUTTON_H
#define BUTTON_H

#include "stdbool.h"

#define BUTTON_PORT 1
#define BUTTON_PIN 6

typedef enum
{
  NO_INPUT,
  HUE_MODIFY,
  SATURATION_MODIFY,
  VALUE_MODIFY
} board_modes_t;

void button_init();

board_modes_t get_board_mode(void);

bool is_mode_changed(void);

#endif // BUTTON_H