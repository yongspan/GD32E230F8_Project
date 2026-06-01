#include "app_brightness.h"
#include "bsp_key.h"
#include "bsp_pwm.h"
#include "bsp_flash.h"
#include "app_battery.h"
#include "app_config.h"
#include <stdio.h>

static const uint8_t brightness_table[] =
{
    0, 1, 2, 3, 5, 8, 12, 18, 25, 35, 50, 70, 100
};

#define BRIGHTNESS_LEVEL_DEFAULT  8U
#define BRIGHTNESS_LEVEL_MAX      ((uint8_t)(sizeof(brightness_table) / sizeof(brightness_table[0]) - 1U))

static uint8_t brightness_level = BRIGHTNESS_LEVEL_DEFAULT;

static void app_brightness_apply(uint8_t save_to_flash)
{
    bsp_pwm_set_duty(brightness_table[brightness_level]);

    if(save_to_flash)
    {
        bsp_flash_write_level(brightness_level);
    }
}

void app_brightness_init(void)
{
    uint8_t saved_level;

    bsp_flash_init();
    saved_level = bsp_flash_read_level();

    if((saved_level != BSP_FLASH_LEVEL_INVALID) && (saved_level <= BRIGHTNESS_LEVEL_MAX))
    {
        brightness_level = saved_level;
#if APP_ENABLE_UART_COMMANDS
        printf("Brightness load from Flash\r\n");
#endif
    }
    else
    {
        brightness_level = BRIGHTNESS_LEVEL_DEFAULT;
#if APP_ENABLE_UART_COMMANDS
        printf("Brightness use default level\r\n");
#endif
    }

    app_brightness_apply(0);

#if APP_ENABLE_UART_COMMANDS
    printf("Brightness init: level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
#endif
}

static void app_brightness_level_up(void)
{
    if(brightness_level < BRIGHTNESS_LEVEL_MAX)
    {
        brightness_level++;
    }

    app_brightness_apply(1);
    app_battery_led_show_for(APP_BATTERY_LED_SHOW_KEY_MS);
#if APP_ENABLE_UART_COMMANDS
    printf("Brightness + : level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
#endif
}

static void app_brightness_level_down(void)
{
    if(brightness_level > 0U)
    {
        brightness_level--;
    }

    app_brightness_apply(1);
    app_battery_led_show_for(APP_BATTERY_LED_SHOW_KEY_MS);
#if APP_ENABLE_UART_COMMANDS
    printf("Brightness - : level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
#endif
}

void app_brightness_process(void)
{
    uint8_t event = bsp_key_get_event();

    /*
     * Use EXTI key events only.
     *
     * rc4 used EXTI + polling fallback at the same time.
     * A single physical key press could first generate an EXTI event, and then
     * be detected again by the polling scan while the key was still pressed.
     * That caused one press to change brightness by two levels.
     */
    if(event == KEY_EVENT_ADD)
    {
        app_brightness_level_up();
    }
    else if(event == KEY_EVENT_SUB)
    {
        app_brightness_level_down();
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

    if(duty > 100U)
    {
        duty = 100U;
    }

    for(i = 0; i <= BRIGHTNESS_LEVEL_MAX; i++)
    {
        uint8_t table_duty = brightness_table[i];
        uint8_t diff = (table_duty > duty) ? (table_duty - duty) : (duty - table_duty);

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
    if(duty > 100U)
    {
        duty = 100U;
    }

    brightness_level = app_brightness_find_nearest_level(duty);
    app_brightness_apply(1);

    app_battery_led_show_for(APP_BATTERY_LED_SHOW_KEY_MS);

#if APP_ENABLE_UART_COMMANDS
    printf("Brightness set duty: input=%d%% level=%d duty=%d%%\r\n",
           duty,
           brightness_level,
           brightness_table[brightness_level]);
#endif
}

void app_brightness_set_level(uint8_t level)
{
    if(level > BRIGHTNESS_LEVEL_MAX)
    {
        level = BRIGHTNESS_LEVEL_MAX;
    }

    brightness_level = level;
    app_brightness_apply(1);

    app_battery_led_show_for(APP_BATTERY_LED_SHOW_KEY_MS);

#if APP_ENABLE_UART_COMMANDS
    printf("Brightness set: level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
#endif
}

void app_brightness_factory_reset(void)
{
    brightness_level = BRIGHTNESS_LEVEL_DEFAULT;
    app_brightness_apply(1);

    app_battery_led_show_for(APP_BATTERY_LED_SHOW_KEY_MS);

#if APP_ENABLE_UART_COMMANDS
    printf("Brightness factory reset: level=%d duty=%d%%\r\n",
           brightness_level,
           brightness_table[brightness_level]);
#endif
}
