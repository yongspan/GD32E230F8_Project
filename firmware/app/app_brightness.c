#include "app_brightness.h"
#include "bsp_key.h"
#include "bsp_pwm.h"
#include <stdio.h>

static const uint8_t brightness_table[] =
{
    0, 1, 2, 3, 5, 8, 12, 18, 25, 35, 50, 70, 100
};

#define BRIGHTNESS_LEVEL_MAX  (sizeof(brightness_table) / sizeof(brightness_table[0]) - 1)

static uint8_t brightness_level = 8;

void app_brightness_init(void)
{
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

        printf("Brightness - : level=%d duty=%d%%\r\n",
               brightness_level,
               brightness_table[brightness_level]);
    }
}