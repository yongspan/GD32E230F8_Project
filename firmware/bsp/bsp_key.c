#include "bsp_key.h"
#include "delay.h"

#define KEY_DEBOUNCE_MS 100

static uint8_t key_add_last_state = KEY_RELEASED;
static uint8_t key_sub_last_state = KEY_RELEASED;
static volatile uint8_t key_event = KEY_EVENT_NONE;
static volatile uint32_t key_add_last_ms = 0;
static volatile uint32_t key_sub_last_ms = 0;
static volatile uint8_t key_irq_pending = 0;

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



void bsp_key_exti_init(void)
{
    bsp_key_init();

    rcu_periph_clock_enable(RCU_CFGCMP);

    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN2);
    syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN3);

    exti_init(EXTI_2, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_init(EXTI_3, EXTI_INTERRUPT, EXTI_TRIG_FALLING);

    exti_interrupt_flag_clear(EXTI_2);
    exti_interrupt_flag_clear(EXTI_3);

    nvic_irq_enable(EXTI2_3_IRQn, 2U);
}

void bsp_key_exti_irq_handler(void)
{
    if(RESET != exti_interrupt_flag_get(EXTI_2))
    {
        exti_interrupt_flag_clear(EXTI_2);
        key_irq_pending = 1;
    }

    if(RESET != exti_interrupt_flag_get(EXTI_3))
    {
        exti_interrupt_flag_clear(EXTI_3);
        key_irq_pending = 1;
    }
}

uint8_t bsp_key_get_irq_pending(void)
{
    if(key_irq_pending)
    {
        key_irq_pending = 0;
        return 1;
    }

    return 0;
}

uint8_t bsp_key_get_event(void)
{
    uint8_t event = key_event;
    key_event = KEY_EVENT_NONE;
    return event;
}
