# Bug Record

## Bug-001

### Problem

LED 不亮。

### Cause

工程默认使用外部晶振 HXTAL。

实际硬件没有外部晶振。

### Solution

修改 system_gd32e23x.c。

使用内部 IRC8M。

---

## Bug-002

### Problem

bsp_led_toggle undeclared。

### Cause

函数已经实现。

头文件没有声明。

### Solution

在 bsp_led.h 增加函数声明。

---

## Bug-003

### Problem

SysTick_Handler multiply defined。

### Cause

delay.c 与 gd32e23x_it.c 同时定义 SysTick_Handler。

### Solution

SysTick_Handler 统一放入 gd32e23x_it.c。

delay.c 提供 delay_decrement()。

---

## Bug-004

### Problem

LED 无法闪烁。

### Cause

未建立 SysTick 延时模块。

### Solution

建立 delay.c / delay.h。

使用 SysTick 实现 1ms Tick。
