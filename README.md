# GD32E230F8_Project
GD32E230F8V6 Embedded Controller Project
# GD32E230F8 Project

## Overview

Embedded firmware project based on GD32E230F8V6 MCU.

Current progress:

* Project structure established
* BSP LED driver completed
* Internal IRC8M clock configured
* LED verified successfully
* GitHub + VS Code + Keil workflow established

---

## Hardware

### MCU

* GD32E230F8V6

### Clock

* Internal IRC8M
* No external crystal

### LED

* PA7
* Active High

### Boot Configuration

* BOOT0 = GND
* NRST pulled up to 3.3V

---

## Development Environment

* Keil uVision
* VS Code
* GitHub Desktop
* GD-Link / ST-Link

---

## Current Features

* BSP LED driver
* System clock initialization
* LED ON test

---

## Next Steps

* SysTick delay module
* LED blink
* UART debug
* Button input
* Timer interrupt
