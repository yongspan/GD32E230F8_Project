#include "gd32e23x.h"
#include "bsp_led.h"
#include "delay.h"

int main(void)
{
    bsp_led_init();
    delay_init();

    while(1)
    {
        bsp_led_toggle();
        delay_ms(500);
    }
}