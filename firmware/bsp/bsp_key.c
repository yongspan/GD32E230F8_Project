#include "bsp_key.h"
#include "delay.h"

static uint8_t key_add_last_state = KEY_RELEASED;
static uint8_t key_sub_last_state = KEY_RELEASED;

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


uint8_t bsp_key_add_scan(void)
{
    uint8_t current_state = bsp_key_add_read();

    if(key_add_last_state == KEY_RELEASED &&
       current_state == KEY_PRESSED)
    {
        delay_ms(20);

        if(bsp_key_add_read() == KEY_PRESSED)
        {
            key_add_last_state = KEY_PRESSED;
            return 1;
        }
    }

    if(current_state == KEY_RELEASED)
    {
        key_add_last_state = KEY_RELEASED;
    }

    return 0;
}

uint8_t bsp_key_sub_scan(void)
{
    uint8_t current_state = bsp_key_sub_read();

    if(key_sub_last_state == KEY_RELEASED &&
       current_state == KEY_PRESSED)
    {
        delay_ms(20);

        if(bsp_key_sub_read() == KEY_PRESSED)
        {
            key_sub_last_state = KEY_PRESSED;
            return 1;
        }
    }

    if(current_state == KEY_RELEASED)
    {
        key_sub_last_state = KEY_RELEASED;
    }

    return 0;
}