#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * Project build mode
 *
 * 1 = debug mode
 *     PA9  = USART0_TX
 *     PA10 = USART0_RX
 *     LED4 and USB_DET are disabled because the pins are used by UART.
 *
 * 0 = product mode
 *     PA9  = Battery LED4
 *     PA10 = USB_DET input
 *     PF0  = SWITCH_DET input
 *     UART command system is disabled.
 */
#define APP_ENABLE_UART_COMMANDS        0U

/* Battery LED behavior */
#define APP_BATTERY_LED_SHOW_BOOT_MS    5000U
#define APP_BATTERY_LED_SHOW_KEY_MS     5000U
#define APP_BATTERY_LED_SHOW_CMD_MS     5000U
#define APP_BATTERY_LED_SHOW_CHARGE_MS  10000U  /* Reserved: charging is continuous until USB removal. */

#endif
