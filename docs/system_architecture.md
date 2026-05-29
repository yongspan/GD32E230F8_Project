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
