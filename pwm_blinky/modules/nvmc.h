#ifndef NVMC_H
#define NVMC_H

#include "nrfx_nvmc.h"
#include "logs.h"

bool nvmc_init(void);

bool flash_has_data(void);

void push_to_flash(uint32_t);

uint32_t pop_from_flash(void);

#endif // NVMC_H