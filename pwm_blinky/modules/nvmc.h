#ifndef NVMC_H
#define NVMC_H

#include "nrfx_nvmc.h"
#include "logs.h"
#include "hsv_rgb.h"

uint32_t cur_list_addr;
uint32_t n_elem;

typedef struct
{
    char name[8];
    uint32_t red;
    uint32_t green;
    uint32_t blue;
} rgb_list;

rgb_list colors_list[10];

uint32_t get_n_elem(void);

uint32_t set_n_elem(uint32_t);

void nvmc_init(Color *);

void read_color(Color *);

void write_color(Color *);

void update_list();

void save_rgb_color(uint32_t red, uint32_t green, uint32_t blue, char color_name[8]);

#endif // NVMC_H