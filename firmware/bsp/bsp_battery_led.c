#include "bsp_battery_led.h"
#include "gd32e23x.h"
#include "app_config.h"

/*
 * Battery LEDs:
 * LED1 = PA1
 * LED2 = PA7
 * LED3 = PB1
 * LED4 = PA9, only available when APP_ENABLE_UART_COMMANDS == 0
 *
 * PA9 is USART0_TX in debug mode. Do not drive PA9 as LED4 while UART is enabled.
 */
#define BAT_LED1_PORT      GPIOA
#define BAT_LED1_PIN       GPIO_PIN_1

#define BAT_LED2_PORT      GPIOA
#define BAT_LED2_PIN       GPIO_PIN_7

#define BAT_LED3_PORT      GPIOB
#define BAT_LED3_PIN       GPIO_PIN_1

#define BAT_LED4_PORT      GPIOA
#define BAT_LED4_PIN       GPIO_PIN_9

static void bat_led_on(uint32_t gpio_periph, uint32_t pin)
{
    gpio_bit_set(gpio_periph, pin);
}

static void bat_led_off(uint32_t gpio_periph, uint32_t pin)
{
    gpio_bit_reset(gpio_periph, pin);
}

uint8_t bsp_battery_led_get_max_count(void)
{
#if APP_ENABLE_UART_COMMANDS
    return 3U;
#else
    return 4U;
#endif
}

void bsp_battery_led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                  BAT_LED1_PIN | BAT_LED2_PIN);

    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP,
                            GPIO_OSPEED_50MHZ,
                            BAT_LED1_PIN | BAT_LED2_PIN);

    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                  BAT_LED3_PIN);

    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP,
                            GPIO_OSPEED_50MHZ,
                            BAT_LED3_PIN);

#if !APP_ENABLE_UART_COMMANDS
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BAT_LED4_PIN);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, BAT_LED4_PIN);
#endif

    bsp_battery_led_all_off();
}

void bsp_battery_led_all_off(void)
{
    bat_led_off(BAT_LED1_PORT, BAT_LED1_PIN);
    bat_led_off(BAT_LED2_PORT, BAT_LED2_PIN);
    bat_led_off(BAT_LED3_PORT, BAT_LED3_PIN);

#if !APP_ENABLE_UART_COMMANDS
    bat_led_off(BAT_LED4_PORT, BAT_LED4_PIN);
#endif
}

void bsp_battery_led_set_count(uint8_t count)
{
    uint8_t max_count = bsp_battery_led_get_max_count();

    if(count > max_count)
    {
        count = max_count;
    }

    bsp_battery_led_all_off();

    if(count >= 1U)
    {
        bat_led_on(BAT_LED1_PORT, BAT_LED1_PIN);
    }

    if(count >= 2U)
    {
        bat_led_on(BAT_LED2_PORT, BAT_LED2_PIN);
    }

    if(count >= 3U)
    {
        bat_led_on(BAT_LED3_PORT, BAT_LED3_PIN);
    }

#if !APP_ENABLE_UART_COMMANDS
    if(count >= 4U)
    {
        bat_led_on(BAT_LED4_PORT, BAT_LED4_PIN);
    }
#endif
}

void bsp_battery_led_set_mask(uint8_t mask)
{
    bsp_battery_led_all_off();

    if(mask & 0x01U)
    {
        bat_led_on(BAT_LED1_PORT, BAT_LED1_PIN);
    }

    if(mask & 0x02U)
    {
        bat_led_on(BAT_LED2_PORT, BAT_LED2_PIN);
    }

    if(mask & 0x04U)
    {
        bat_led_on(BAT_LED3_PORT, BAT_LED3_PIN);
    }

#if !APP_ENABLE_UART_COMMANDS
    if(mask & 0x08U)
    {
        bat_led_on(BAT_LED4_PORT, BAT_LED4_PIN);
    }
#else
    (void)mask;
#endif
}
