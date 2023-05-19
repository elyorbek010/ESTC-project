#ifndef FSTORAGE_H
#define FSTORAGE_H

#define COLOR_NAME_LENGTH 20

#include "stdint.h"

uint32_t list_cur_addr;
uint32_t n_list_elem;

typedef struct
{
    char name[COLOR_NAME_LENGTH];
    uint32_t red;
    uint32_t green;
    uint32_t blue;
} rgb_list;

rgb_list colors_list[10];

void fstorage_init();

void save_color();

void save_rgb_color(uint32_t red, uint32_t green, uint32_t blue, char color_name[COLOR_NAME_LENGTH]);

void update_list(void);

#endif // FSTORAGE_H