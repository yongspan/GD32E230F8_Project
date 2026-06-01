# System Architecture

## Project Structure

GD32E23x

├─ Application

│ ├─ main.c

│ └─ gd32e23x_it.c

│

├─ BSP

│ ├─ bsp_led.c

│ ├─ bsp_led.h

│ ├─ delay.c

│ └─ delay.h

│

├─ CMSIS

│ └─ system_gd32e23x.c

│

├─ Peripherals

│ ├─ gd32e23x_gpio.c

│ ├─ gd32e23x_rcu.c

│ ├─ gd32e23x_misc.c

│ └─ ...

│

└─ Startup

└─ startup_gd32e23x.s

## File Responsibilities

### startup_gd32e23x.s

系统启动文件。

### system_gd32e23x.c

系统时钟配置文件。

### gd32e23x_it.c

中断入口文件。

### main.c

应用程序入口。

### bsp_led.c

LED 驱动模块。

### delay.c

SysTick 延时模块。

## Design Principle

官方库尽量不修改。

所有用户代码放入 BSP 或 Application。

中断入口统一放在 gd32e23x_it.c。

硬件驱动统一放入 BSP。
Current Verified Functions

- System clock (IRC8M)
- GPIO output
- LED driver
- SysTick interrupt
- Delay module
- LED blink (500ms)

## Current Verified Functions

### System

* Internal IRC8M clock
* SystemCoreClock configured correctly
* SysTick 1ms interrupt

### GPIO

* PA7 LED output
* LED active high

### Delay

* delay_ms()
* SysTick based software delay

### UART

* USART0
* PA9 TX
* PA10 RX
* 115200 baudrate
* printf redirect
* UART TX verified through DAPLink VCP

### Debug

* LED debug
* UART log debug
* Git version control

UART

√ USART0 TX
√ USART0 RX
√ printf redirect
√ Polling receive
UART

√ TX
√ RX Polling
√ RX Interrupt
UART Module

USART0
PA9 TX
PA10 RX

Features

√ TX
√ RX Polling
√ RX Interrupt
√ Ring Buffer
√ printf Redirect
Command Parser

Features

√ led on;
√ led off;

Architecture

UART RX
↓
Interrupt
↓
Ring Buffer
↓
Command Parser
↓
LED Control
UART Command Parser

Command End Symbol

;

Supported Commands

led1 on;
led1 off;

led2 on;
led2 off;

blink1;

all off;

status;

help;

# Backlight Module

## Overview

Backlight brightness is controlled by TPS61165 LED driver.

The MCU outputs a PWM signal to the TPS61165 CTRL pin.

## Hardware Resource

| Item               | Resource     |
| ------------------ | ------------ |
| MCU                | GD32E230F8V6 |
| Driver IC          | TPS61165     |
| PWM GPIO           | PA6          |
| Timer              | TIMER2_CH0   |
| Alternate Function | GPIO_AF_1    |

## PWM Configuration

| Item          | Value  |
| ------------- | ------ |
| PWM Frequency | 10kHz  |
| Duty Range    | 0~100% |

## Software Interface

```c
void bsp_pwm_init(void);
void bsp_pwm_set_duty(uint8_t duty);
uint8_t bsp_pwm_get_duty(void);
```

## Notes

TPS61165 brightness is controlled through PWM signal applied to CTRL pin.

Recommended PWM dimming frequency:

6.5kHz ~ 100kHz

Current implementation:

10kHz

```
```
# System Architecture

Project: GD32E230F8 Backlight Controller
MCU: GD32E230F8V6
Version: v1.0.0-rc1

---

## 1. Project Overview

This project is a backlight controller based on the GD32E230F8V6 MCU.

The controller is used to drive a 17PM backlight module through a TPS61165 LED boost driver.

Main firmware functions:

1. PWM backlight brightness control
2. Key brightness adjustment
3. UART command system
4. Flash brightness save and restore
5. Battery voltage ADC measurement
6. Battery percentage calculation
7. 3-LED battery level display
8. CHRG / STDBY charging status detection
9. Charging LED animation

Current stable firmware baseline:

```text
v1.0.0-rc1
```

---

## 2. Hardware Resource Summary

| Function | MCU Pin | Firmware Usage                      |
| -------- | ------- | ----------------------------------- |
| BAT_ADC  | PA0     | ADC input for battery voltage       |
| LED1     | PA1     | Battery level LED 1                 |
| KEY_ADD  | PA2     | Brightness increase key             |
| KEY_SUB  | PA3     | Brightness decrease key             |
| CHRG     | PA4     | Charger charging status input       |
| STDBY    | PA5     | Charger full / standby status input |
| PWM_CTRL | PA6     | PWM output to TPS61165 CTRL         |
| LED2     | PA7     | Battery level LED 2                 |
| LED3     | PB1     | Battery level LED 3                 |
| UART_TX  | PA9     | USART0 TX debug output              |
| UART_RX  | PA10    | USART0 RX debug command input       |

Important current decisions:

1. PA6 is used as PWM output.
2. PA9 is reserved for UART TX.
3. PA10 is reserved for UART RX.
4. USB_DET is not enabled in the debug firmware.
5. LED4 is not used in the current firmware.
6. Battery level display uses only 3 LEDs.

---

## 3. Firmware Layer Architecture

The firmware is divided into several layers.

```text
Application Layer
    |
    |-- app_brightness
    |-- app_battery
    |-- app_charge
    |-- cmd_parser
    |
BSP Layer
    |
    |-- bsp_pwm
    |-- bsp_key
    |-- bsp_uart
    |-- bsp_flash
    |-- bsp_adc
    |-- bsp_led
    |-- bsp_charge
    |
Driver / Vendor Library Layer
    |
    |-- GD32E23x Standard Peripheral Library
    |-- CMSIS
    |
Hardware Layer
    |
    |-- GD32E230F8V6
    |-- TPS61165
    |-- TC4056A
    |-- Battery
    |-- LEDs
    |-- Keys
```

---

## 4. Module Responsibility

### 4.1 `main.c`

`main.c` is responsible for system initialization and main loop scheduling.

Main responsibilities:

1. Initialize system clock
2. Initialize delay / SysTick
3. Initialize UART
4. Initialize PWM
5. Initialize keys
6. Initialize ADC
7. Initialize LEDs
8. Initialize charge status input
9. Load brightness level from Flash
10. Run periodic tasks in the main loop

Typical flow:

```c
int main(void)
{
    system_init();

    while (1)
    {
        key_task();
        brightness_task();
        battery_task();
        charge_task();
        led_display_task();
        uart_command_task();
    }
}
```

---

### 4.2 `bsp_pwm`

`bsp_pwm` controls the PWM output to the TPS61165 CTRL pin.

Current PWM resource:

| Item          | Value      |
| ------------- | ---------- |
| PWM Pin       | PA6        |
| Timer         | TIMER2_CH0 |
| PWM Frequency | 10 kHz     |
| Duty Range    | 0% ~ 100%  |

Main responsibilities:

1. Initialize PA6 as timer PWM output
2. Initialize TIMER2_CH0
3. Set PWM duty
4. Stop or disable PWM when required

Suggested interface:

```c
void bsp_pwm_init(void);
void bsp_pwm_set_duty(uint8_t duty);
uint8_t bsp_pwm_get_duty(void);
```

---

### 4.3 `app_brightness`

`app_brightness` manages the logical brightness level.

It should not directly operate hardware registers.
It should call `bsp_pwm_set_duty()` to apply brightness.

Main responsibilities:

1. Maintain current brightness level
2. Convert brightness level to PWM duty
3. Increase brightness level
4. Decrease brightness level
5. Set brightness to a specified level
6. Save brightness level to Flash
7. Restore brightness level from Flash

Suggested brightness table:

```c
static const uint8_t brightness_table[] =
{
    5, 10, 20, 35, 50, 70, 85, 100
};
```

Suggested interface:

```c
void app_brightness_init(void);
void app_brightness_set_level(uint8_t level);
void app_brightness_add(void);
void app_brightness_sub(void);
void app_brightness_save(void);
uint8_t app_brightness_get_level(void);
uint8_t app_brightness_get_duty(void);
```

---

### 4.4 `bsp_key`

`bsp_key` reads the physical keys.

Current key mapping:

| Key     | MCU Pin | Function            |
| ------- | ------- | ------------------- |
| KEY_ADD | PA2     | Brightness increase |
| KEY_SUB | PA3     | Brightness decrease |

Main responsibilities:

1. Initialize key GPIO
2. Read key input level
3. Debounce key input
4. Generate key events

Suggested key events:

```c
KEY_EVENT_NONE
KEY_EVENT_ADD
KEY_EVENT_SUB
```

Suggested interface:

```c
void bsp_key_init(void);
key_event_t bsp_key_scan(void);
```

---

### 4.5 `bsp_uart`

`bsp_uart` provides UART communication for debugging and command input.

Current UART mapping:

| Signal    | MCU Pin |
| --------- | ------- |
| USART0_TX | PA9     |
| USART0_RX | PA10    |

Main responsibilities:

1. Initialize USART0
2. Provide printf output
3. Receive UART data
4. Use interrupt RX if enabled
5. Push received bytes into ring buffer

Suggested interface:

```c
void bsp_uart_init(void);
void bsp_uart_send_char(char ch);
void bsp_uart_send_string(const char *str);
```

---

### 4.6 `bsp_uart_ringbuffer`

`bsp_uart_ringbuffer` stores UART RX data.

Main responsibilities:

1. Store received UART bytes
2. Avoid data loss during interrupt reception
3. Provide data to command parser

Suggested interface:

```c
void ringbuffer_init(void);
uint8_t ringbuffer_push(uint8_t data);
uint8_t ringbuffer_pop(uint8_t *data);
uint8_t ringbuffer_is_empty(void);
```

---

### 4.7 `cmd_parser`

`cmd_parser` parses UART commands.

Main responsibilities:

1. Read bytes from UART ring buffer
2. Assemble command string
3. Detect command ending symbol `;`
4. Execute matched command
5. Print command result

Current / suggested commands:

```text
help;
bl get;
bl set 5;
bl add;
bl sub;
bl save;
bl default;
bat get;
chg get;
```

Command responsibilities:

| Command       | Function                                |
| ------------- | --------------------------------------- |
| `help;`       | Print command list                      |
| `bl get;`     | Print current brightness level and duty |
| `bl set n;`   | Set brightness level                    |
| `bl add;`     | Increase brightness level               |
| `bl sub;`     | Decrease brightness level               |
| `bl save;`    | Save brightness level to Flash          |
| `bl default;` | Restore default brightness              |
| `bat get;`    | Print battery voltage and percentage    |
| `chg get;`    | Print charging status                   |

---

### 4.8 `bsp_flash`

`bsp_flash` saves user parameters into internal Flash.

Current saved parameter:

```text
brightness_level
```

Current Flash design:

```text
Storage area: last 1KB Flash
Start address: 0x0800FC00
End address:   0x0800FFFF
Magic value:   0x5AA55AA5
```

Main responsibilities:

1. Erase parameter page
2. Write magic value
3. Write brightness level
4. Read saved brightness level
5. Check whether saved data is valid

Suggested data structure:

```c
typedef struct
{
    uint32_t magic;
    uint32_t brightness_level;
} flash_param_t;
```

Suggested interface:

```c
void bsp_flash_save_brightness(uint8_t level);
uint8_t bsp_flash_load_brightness(uint8_t *level);
```

---

### 4.9 `bsp_adc`

`bsp_adc` reads the battery voltage divider signal.

Current ADC resource:

| Item          | Value       |
| ------------- | ----------- |
| ADC Pin       | PA0         |
| Signal        | BAT_ADC     |
| Divider       | 100k / 100k |
| Divider Ratio | About 1:1   |

Battery voltage formula:

```c
BAT_mV = ADC_mV * (R_UP + R_DOWN) / R_DOWN
```

For current 100k / 100k divider:

```c
BAT_mV = ADC_mV * 2
```

Main responsibilities:

1. Initialize ADC
2. Read ADC raw value
3. Convert raw ADC value to ADC pin voltage
4. Convert ADC pin voltage to battery voltage

Suggested interface:

```c
void bsp_adc_init(void);
uint16_t bsp_adc_read_raw(void);
uint16_t bsp_adc_read_battery_mv(void);
```

---

### 4.10 `app_battery`

`app_battery` converts battery voltage to battery percentage and display level.

Main responsibilities:

1. Read battery voltage from `bsp_adc`
2. Calculate battery percentage
3. Convert percentage to LED display level
4. Provide battery status to other modules

Suggested battery level mapping:

| Battery Voltage | Display     |
| --------------- | ----------- |
| < 3.4V          | Low battery |
| 3.4V ~ 3.6V     | 1 LED       |
| 3.6V ~ 3.8V     | 2 LEDs      |
| 3.8V ~ 4.0V     | 3 LEDs      |
| > 4.0V          | Full        |

Current firmware uses 3 battery LEDs.

Suggested interface:

```c
void app_battery_init(void);
void app_battery_task(void);
uint16_t app_battery_get_mv(void);
uint8_t app_battery_get_percent(void);
uint8_t app_battery_get_led_level(void);
```

---

### 4.11 `bsp_charge`

`bsp_charge` reads CHRG and STDBY input pins.

Current charge status pins:

| Signal | MCU Pin | Active Level |
| ------ | ------- | ------------ |
| CHRG   | PA4     | Low          |
| STDBY  | PA5     | Low          |

Charge status logic:

| CHRG | STDBY | Status       |
| ---- | ----- | ------------ |
| Low  | High  | Charging     |
| High | Low   | Full         |
| High | High  | Not charging |
| Low  | Low   | Unknown      |

Suggested charge status enum:

```c
typedef enum
{
    CHARGE_STATUS_NOT_CHARGING = 0,
    CHARGE_STATUS_CHARGING,
    CHARGE_STATUS_FULL,
    CHARGE_STATUS_UNKNOWN
} charge_status_t;
```

Suggested interface:

```c
void bsp_charge_init(void);
charge_status_t bsp_charge_get_status(void);
```

---

### 4.12 `bsp_led`

`bsp_led` controls the battery level LEDs.

Current LED mapping:

| LED  | MCU Pin | Usage         |
| ---- | ------- | ------------- |
| LED1 | PA1     | Battery LED 1 |
| LED2 | PA7     | Battery LED 2 |
| LED3 | PB1     | Battery LED 3 |
| LED4 | PA9     | Not used      |

Main responsibilities:

1. Initialize LED GPIO
2. Turn LED on
3. Turn LED off
4. Display battery level
5. Display charging animation

Suggested interface:

```c
void bsp_led_init(void);
void bsp_led_set_level(uint8_t level);
void bsp_led_charging_animation_task(void);
void bsp_led_all_off(void);
```

---

## 5. Main Loop Flow

The main loop should be simple and clear.

Recommended main loop:

```c
while (1)
{
    cmd_parser_task();
    app_brightness_task();
    app_key_task();
    app_battery_task();
    app_charge_task();
    app_led_display_task();
}
```

Recommended rule:

1. Interrupts should only do short work.
2. UART interrupt should only push data into ring buffer.
3. Main loop should parse commands.
4. Main loop should update display and status.
5. Time-consuming logic should not be placed inside interrupts.

---

## 6. Brightness Control Flow

### 6.1 Key Brightness Control

```text
KEY_ADD pressed
    ↓
bsp_key detects key event
    ↓
app_brightness_add()
    ↓
brightness_level increases
    ↓
brightness_table[level] gets duty
    ↓
bsp_pwm_set_duty(duty)
    ↓
UART prints current level and duty
```

```text
KEY_SUB pressed
    ↓
bsp_key detects key event
    ↓
app_brightness_sub()
    ↓
brightness_level decreases
    ↓
brightness_table[level] gets duty
    ↓
bsp_pwm_set_duty(duty)
    ↓
UART prints current level and duty
```

---

### 6.2 UART Brightness Control

```text
UART receives "bl set 5;"
    ↓
USART0 RX interrupt receives bytes
    ↓
RingBuffer stores bytes
    ↓
cmd_parser_task() reads bytes
    ↓
cmd_parser detects full command
    ↓
app_brightness_set_level(5)
    ↓
bsp_pwm_set_duty(brightness_table[5])
    ↓
UART prints result
```

---

### 6.3 Flash Brightness Restore

```text
Power on
    ↓
System init
    ↓
app_brightness_init()
    ↓
bsp_flash_load_brightness()
    ↓
Check magic value
    ↓
If valid: use saved brightness level
If invalid: use default brightness level
    ↓
bsp_pwm_set_duty()
```

---

### 6.4 Flash Brightness Save

```text
Brightness changed
    ↓
User executes "bl save;"
    ↓
cmd_parser calls app_brightness_save()
    ↓
bsp_flash_save_brightness(level)
    ↓
Erase Flash parameter page
    ↓
Write magic value and brightness level
    ↓
UART prints save result
```

---

## 7. Battery Measurement Flow

```text
app_battery_task()
    ↓
bsp_adc_read_raw()
    ↓
Convert raw ADC value to ADC_mV
    ↓
Convert ADC_mV to BAT_mV
    ↓
Calculate battery percentage
    ↓
Calculate LED display level
    ↓
Update battery status
```

Battery voltage calculation:

```c
ADC_mV = raw * 3300 / 4095
BAT_mV = ADC_mV * (R_UP + R_DOWN) / R_DOWN
```

For 100k / 100k divider:

```c
BAT_mV = ADC_mV * 2
```

---

## 8. Charge Status Flow

```text
app_charge_task()
    ↓
bsp_charge_get_status()
    ↓
Read CHRG pin
Read STDBY pin
    ↓
Determine charge status
    ↓
Update LED display mode
```

Charge status priority:

1. If charging, show charging LED animation.
2. If full, show full battery LED status.
3. If not charging, show battery level.
4. If unknown, keep safe display or print warning.

---

## 9. LED Display Flow

The LED display depends on charge status and battery status.

Recommended priority:

```text
Charging status has higher priority than normal battery display.
```

Display logic:

```text
If charging:
    show charging animation

Else if full:
    show all 3 LEDs on

Else:
    show battery level according to battery percentage
```

Current limitation:

```text
Only 3 LEDs are used.
LED4 is disabled because PA9 is reserved for UART TX.
```

---

## 10. UART Command Flow

```text
PC sends command
    ↓
USART0 RX receives bytes
    ↓
RX interrupt pushes bytes to ring buffer
    ↓
cmd_parser_task() reads bytes
    ↓
Command ends with ';'
    ↓
Parser executes command
    ↓
Firmware prints response
```

Important rules:

1. Commands must end with `;`.
2. UART RX interrupt should not parse commands directly.
3. Parser should run in the main loop.
4. Commands should call application-layer functions, not directly operate hardware registers.

Correct example:

```c
cmd_parser -> app_brightness_set_level() -> bsp_pwm_set_duty()
```

Avoid this:

```c
cmd_parser -> TIMER register operation
```

---

## 11. System Timing Design

Recommended periodic task timing:

| Task                   | Suggested Period    |
| ---------------------- | ------------------- |
| Key scan               | 10 ms               |
| UART command parser    | As fast as possible |
| Battery ADC update     | 500 ms ~ 1000 ms    |
| Charge status update   | 200 ms ~ 500 ms     |
| LED charging animation | 200 ms ~ 500 ms     |
| Flash save             | Only when requested |

Important rule:

```text
Do not write Flash repeatedly in a fast loop.
```

Flash should only be written when:

1. User sends save command
2. Firmware decides to save after brightness changes with delay
3. Before power-down, if power-down detection is implemented in the future

---

## 12. Debug Firmware vs Production Firmware

### 12.1 Debug Firmware

Current firmware is a debug-friendly firmware.

Debug firmware keeps:

1. UART TX on PA9
2. UART RX on PA10
3. Command system enabled
4. LED4 disabled
5. USB_DET disabled

Purpose:

1. Easier development
2. Easier testing
3. Easier problem diagnosis
4. Easier GPT / Codex assisted debugging

---

### 12.2 Production Firmware

Future production firmware may change pin usage.

Possible changes:

1. Disable UART command system
2. Reuse PA9 as LED4
3. Reuse PA10 as USB_DET
4. Enable 4-LED battery display
5. Enable USB insertion detection

Important:

```text
Production firmware must have a separate pin map and configuration.
Do not mix debug firmware pin assignment with production firmware pin assignment.
```

---

## 13. Current Stable Baseline

Current stable tag:

```text
v1.0.0-rc1
```

Verified functions:

1. PWM backlight control
2. Key brightness adjustment
3. UART command system
4. Flash brightness save and restore
5. ADC battery voltage measurement
6. Battery percentage calculation
7. 3-LED battery display
8. CHRG / STDBY charging status detection
9. Charging LED animation

This version can be used as the baseline before preparing the formal `v1.0.0` release.

---

## 14. Future Development Direction

Recommended next steps:

1. Clean up command reference documentation
2. Clean up firmware module documentation
3. Create test report for `v1.0.0-rc1`
4. Confirm all known hardware limitations
5. Prepare `v1.0.0` formal release
6. Decide whether to keep debug firmware and production firmware separately

Recommended future documents:

```text
docs/command_reference.md
docs/firmware_modules.md
docs/test_report_v1.0.0-rc1.md
docs/release_note_v1.0.0.md
docs/debug_vs_production_firmware.md
```

---

## 15. Development Rules for GPT / Codex

When modifying this project, follow these rules:

1. Do not change the pin map unless explicitly requested.
2. PA6 must remain PWM output.
3. PA9 must remain UART TX in debug firmware.
4. PA10 must remain UART RX in debug firmware.
5. LED4 must remain disabled in debug firmware.
6. USB_DET must remain disabled in debug firmware.
7. Do not write Flash repeatedly in the main loop.
8. Keep interrupt handlers short.
9. Keep hardware register operations inside BSP modules.
10. Keep product logic inside APP modules.
11. Keep command parsing inside `cmd_parser`.
12. Update documentation after changing firmware behavior.
