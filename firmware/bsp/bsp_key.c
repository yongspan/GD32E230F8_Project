#include "bsp_key.h"

void bsp_key_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_mode_set(GPIOA,
                  GPIO_MODE_INPUT,
                  GPIO_PUPD_PULLUP,
                  KEY_ADD_GPIO_PIN | KEY_SUB_GPIO_PIN);
}

uint8_t bsp_key_add_read(void)
{
    return gpio_input_bit_get(KEY_ADD_GPIO_PORT, KEY_ADD_GPIO_PIN);
}

uint8_t bsp_key_sub_read(void)
{
    return gpio_input_bit_get(KEY_SUB_GPIO_PORT, KEY_SUB_GPIO_PIN);
}