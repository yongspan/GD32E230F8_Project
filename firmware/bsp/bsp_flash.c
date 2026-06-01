#include "bsp_flash.h"
#include "gd32e23x_fmc.h"

#define BSP_FLASH_PAGE_SIZE       1024U

/* GD32E230F8V6: 64KB Flash
 * Flash range: 0x08000000 ~ 0x0800FFFF
 * Last 1KB page: 0x0800FC00 ~ 0x0800FFFF
 */
#define BSP_FLASH_SAVE_ADDR       0x0800FC00U

#define BSP_FLASH_MAGIC           0x5AA55AA5U

typedef struct
{
    uint32_t magic;
    uint32_t level;
} bsp_flash_cfg_t;

void bsp_flash_init(void)
{
    /* ???????? */
}

uint8_t bsp_flash_read_level(void)
{
    bsp_flash_cfg_t *cfg = (bsp_flash_cfg_t *)BSP_FLASH_SAVE_ADDR;

    if(cfg->magic != BSP_FLASH_MAGIC)
    {
        return BSP_FLASH_LEVEL_INVALID;
    }

    if(cfg->level > 12U)
    {
        return BSP_FLASH_LEVEL_INVALID;
    }

    return (uint8_t)cfg->level;
}

void bsp_flash_write_level(uint8_t level)
{
    if(level > 12U)
    {
        return;
    }

    bsp_flash_cfg_t *cfg = (bsp_flash_cfg_t *)BSP_FLASH_SAVE_ADDR;

    if(cfg->magic == BSP_FLASH_MAGIC && cfg->level == level)
    {
        return;
    }

    fmc_unlock();

    fmc_page_erase(BSP_FLASH_SAVE_ADDR);
    fmc_word_program(BSP_FLASH_SAVE_ADDR, BSP_FLASH_MAGIC);
    fmc_word_program(BSP_FLASH_SAVE_ADDR + 4U, (uint32_t)level);

    fmc_lock();
}