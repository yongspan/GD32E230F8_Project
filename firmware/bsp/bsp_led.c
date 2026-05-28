#include "bsp_led.h"

void bsp_led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                  LED1_GPIO_PIN | LED2_GPIO_PIN);

    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP,
                            GPIO_OSPEED_50MHZ,
                            LED1_GPIO_PIN | LED2_GPIO_PIN);

    gpio_bit_reset(GPIOA, LED1_GPIO_PIN | LED2_GPIO_PIN); // ???
}

void bsp_led1_on(void)
{
    gpio_bit_set(LED1_GPIO_PORT, LED1_GPIO_PIN);          // ????
}

void bsp_led1_off(void)
{
    gpio_bit_reset(LED1_GPIO_PORT, LED1_GPIO_PIN);
}