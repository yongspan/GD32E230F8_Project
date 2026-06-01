#include "bsp_battery_led.h"
#include "gd32e23x.h"

/*
 * Battery LEDs:
 *
 * LED1 = PA1
 * LED2 = PA7
 * LED3 = PB1
 *
 * LED4 = PA9 is NOT used now.
 * PA9 is reserved for USART0_TX.
 */

#define BAT_LED1_PORT      GPIOA
#define BAT_LED1_PIN       GPIO_PIN_1

#define BAT_LED2_PORT      GPIOA
#define BAT_LED2_PIN       GPIO_PIN_7

#define BAT_LED3_PORT      GPIOB
#define BAT_LED3_PIN       GPIO_PIN_1

static void bat_led_on(uint32_t gpio_periph, uint32_t pin)
{
    gpio_bit_set(gpio_periph, pin);
}

static void bat_led_off(uint32_t gpio_periph, uint32_t pin)
{
    gpio_bit_reset(gpio_periph, pin);
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

    bsp_battery_led_all_off();
}

void bsp_battery_led_all_off(void)
{
    bat_led_off(BAT_LED1_PORT, BAT_LED1_PIN);
    bat_led_off(BAT_LED2_PORT, BAT_LED2_PIN);
    bat_led_off(BAT_LED3_PORT, BAT_LED3_PIN);
}

void bsp_battery_led_set_count(uint8_t count)
{
    if(count > 3)
    {
        count = 3;
    }

    bsp_battery_led_all_off();

    if(count >= 1)
    {
        bat_led_on(BAT_LED1_PORT, BAT_LED1_PIN);
    }

    if(count >= 2)
    {
        bat_led_on(BAT_LED2_PORT, BAT_LED2_PIN);
    }

    if(count >= 3)
    {
        bat_led_on(BAT_LED3_PORT, BAT_LED3_PIN);
    }
}

void bsp_battery_led_set_mask(uint8_t mask)
{
    bsp_battery_led_all_off();

    if(mask & 0x01)
    {
        gpio_bit_set(GPIOA, GPIO_PIN_1);   // LED1
    }

    if(mask & 0x02)
    {
        gpio_bit_set(GPIOA, GPIO_PIN_7);   // LED2
    }

    if(mask & 0x04)
    {
        gpio_bit_set(GPIOB, GPIO_PIN_1);   // LED3
    }
}
