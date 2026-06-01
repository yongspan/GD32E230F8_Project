#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include "gd32e23x.h"
#include <stdint.h>

#define BSP_FLASH_LEVEL_INVALID   0xFFU

void bsp_flash_init(void);

uint8_t bsp_flash_read_level(void);
void bsp_flash_write_level(uint8_t level);

#endif