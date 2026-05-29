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
