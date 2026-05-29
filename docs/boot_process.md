# Boot Process

## Power On

MCU 上电后并不会直接进入 main()。

启动流程如下：

Power On / Reset

↓

startup_gd32e23x.s

↓

Reset_Handler

↓

SystemInit()

↓

system_gd32e23x.c

↓

main()

↓

bsp_led_init()

↓

delay_init()

↓

while(1)

## startup_gd32e23x.s

作用：

* 设置栈顶指针 SP
* 建立中断向量表
* 调用 SystemInit()
* 调用 main()

一般情况下不修改。

## system_gd32e23x.c

作用：

* 配置系统时钟
* 更新 SystemCoreClock
* 配置系统运行环境

当前工程：

* 使用内部 IRC8M
* 无外部晶振

## main()

应用程序入口。

当前完成：

* LED 初始化
* SysTick 初始化
* LED 500ms 闪烁
