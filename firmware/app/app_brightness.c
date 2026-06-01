#include "app_brightness.h"
#include "bsp_key.h"
#include "bsp_pwm.h"
#include <stdio.h>
#include "bsp_flash.h"

static const uint8_t brightness_table[] =
{
    0, 1, 2, 3, 5, 8, 12, 18, 25, 35, 50, 70, 100
};

#define BRIGHTNESS_LEVEL_MAX  (sizeof(brightness_table) / sizeof(brightness_table[0]) - 1)

static uint8_t brightness_level = 8;

void app_brightness_init(void)
{
    uint8_t saved_level;

    bsp_flash_init();

    saved_level = bsp_flash_read_level();

    if(saved_level != BSP_FLASH_LEVEL_INVALID &&
       saved_level <= BRIGHTNESS_LEVEL_MAX)
    {
        brightness_level = saved_level;
        printf("Brightness load from Flash\r\n");
    }
    else
    {
        printf("Brightness use default level\r\n");
    }

    bsp_pwm_set_duty(brightness_table[brightness_level]);
    
    printf("Brightness init: level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
}

void app_brightness_process(void)
{
    if(bsp_key_add_scan())
    {
        if(brightness_level < BRIGHTNESS_LEVEL_MAX)
        {
            brightness_level++;
        }

        bsp_pwm_set_duty(brightness_table[brightness_level]);
        bsp_flash_write_level(brightness_level);
        printf("Brightness + : level=%d duty=%d%%\r\n",
               brightness_level,
               brightness_table[brightness_level]);
    }

    if(bsp_key_sub_scan())
    {
        if(brightness_level > 0)
        {
            brightness_level--;
        }

        bsp_pwm_set_duty(brightness_table[brightness_level]);
        bsp_flash_write_level(brightness_level);
        printf("Brightness - : level=%d duty=%d%%\r\n",
               brightness_level,
               brightness_table[brightness_level]);
    }
}
uint8_t app_brightness_get_level(void)
{
    return brightness_level;
}

uint8_t app_brightness_get_duty(void)
{
    return brightness_table[brightness_level];
}

uint8_t app_brightness_get_max_level(void)
{
    return BRIGHTNESS_LEVEL_MAX;
}

static uint8_t app_brightness_find_nearest_level(uint8_t duty)
{
    uint8_t i;
    uint8_t best_level = 0;
    uint8_t best_diff = 255;

    if(duty > 100)
    {
        duty = 100;
    }

    for(i = 0; i <= BRIGHTNESS_LEVEL_MAX; i++)
    {
        uint8_t table_duty = brightness_table[i];
        uint8_t diff;

        if(table_duty > duty)
        {
            diff = table_duty - duty;
        }
        else
        {
            diff = duty - table_duty;
        }

        if(diff < best_diff)
        {
            best_diff = diff;
            best_level = i;
        }
    }

    return best_level;
}

void app_brightness_set_duty(uint8_t duty)
{
    uint8_t level;

    if(duty > 100)
    {
        duty = 100;
    }

    level = app_brightness_find_nearest_level(duty);

    brightness_level = level;

    bsp_pwm_set_duty(brightness_table[brightness_level]);
    bsp_flash_write_level(brightness_level);

    printf("Brightness set duty: input=%d%% level=%d duty=%d%%\r\n",
           duty,
           brightness_level,
           brightness_table[brightness_level]);
}


void app_brightness_set_level(uint8_t level)
{
    if(level > BRIGHTNESS_LEVEL_MAX)
    {
        level = BRIGHTNESS_LEVEL_MAX;
    }

    brightness_level = level;

    bsp_pwm_set_duty(brightness_table[brightness_level]);
    bsp_flash_write_level(brightness_level);

    printf("Brightness set: level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
}

void app_brightness_factory_reset(void)
{
    brightness_level = 8;

    bsp_pwm_set_duty(brightness_table[brightness_level]);
    bsp_flash_write_level(brightness_level);

    printf("Brightness factory reset: level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
}