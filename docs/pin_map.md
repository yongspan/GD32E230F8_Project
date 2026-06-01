# Pin Map

Project: GD32E230F8 Backlight Controller
MCU: GD32E230F8V6
Version: v1.0.0-rc1

---

## 1. Current Pin Assignment

| Function | MCU Pin | Description                            |
| -------- | ------- | -------------------------------------- |
| BAT_ADC  | PA0     | Battery voltage divider ADC input      |
| LED1     | PA1     | Battery level LED 1                    |
| KEY_ADD  | PA2     | Brightness increase key                |
| KEY_SUB  | PA3     | Brightness decrease key                |
| CHRG     | PA4     | Charger status input, low = charging   |
| STDBY    | PA5     | Charger standby/full input, low = full |
| PWM_CTRL | PA6     | PWM output to TPS61165 CTRL pin        |
| LED2     | PA7     | Battery level LED 2                    |
| LED3     | PB1     | Battery level LED 3                    |
| UART_TX  | PA9     | USART0 TX, reserved for debug          |
| UART_RX  | PA10    | USART0 RX, reserved for debug          |

---

## 2. Important Hardware Notes

### 2.1 PA6 PWM

The original PWM pin was PA1, but PA1 does not have a suitable timer channel for the current PWM implementation.

The PWM signal was moved by fly wire to PA6.

Current PWM resource:

| Item          | Value               |
| ------------- | ------------------- |
| PWM Pin       | PA6                 |
| Timer         | TIMER2_CH0          |
| Function      | TPS61165 CTRL input |
| PWM Frequency | 10 kHz              |
| Duty Range    | 0% ~ 100%           |

Firmware decision:

```text
PA6 = PWM_CTRL
Timer = TIMER2_CH0
Do not use PA6 as battery LED output.
```

---

### 2.2 PA9 UART TX and LED4 Conflict

PA9 is currently used as USART0 TX.

Although PA9 was considered for LED4, it is reserved for UART debug in the current firmware.

Current decision:

```text
PA9 = UART TX
LED4 = Not used
Do not configure PA9 as GPIO LED output in the debug firmware.
```

Reason:

* UART debug is still important for firmware development.
* PA9 must remain USART0 TX.
* LED4 is disabled in the current firmware.

---

### 2.3 PA10 UART RX and USB_DET Conflict

PA10 is connected to USB_DET in hardware, but PA10 is also used as USART0 RX.

Current decision:

```text
PA10 = UART RX
USB_DET = Not enabled in debug firmware
```

Reason:

* UART RX is required for the command system.
* USB insertion status is not used in v1.0.0-rc1.
* Charging status is detected through CHRG and STDBY instead.

---

## 3. Battery LED Mapping

Only 3 battery LEDs are used in the current firmware.

| Battery LED | MCU Pin | Status                         |
| ----------- | ------- | ------------------------------ |
| LED1        | PA1     | Used                           |
| LED2        | PA7     | Used                           |
| LED3        | PB1     | Used                           |
| LED4        | PA9     | Not used, reserved for UART TX |

Current firmware decision:

```text
Battery display uses 3 LEDs only.
PA9 must not be used as LED4 while UART debug is enabled.
```

---

## 4. Charge Status Pins

| Signal | MCU Pin | Active Level | Meaning                   |
| ------ | ------- | ------------ | ------------------------- |
| CHRG   | PA4     | Low          | Battery is charging       |
| STDBY  | PA5     | Low          | Battery is full / standby |

Charge status logic:

| CHRG | STDBY | Firmware Status |
| ---- | ----- | --------------- |
| Low  | High  | Charging        |
| High | Low   | Full            |
| High | High  | Not charging    |
| Low  | Low   | Unknown         |

Firmware decision:

```text
CHRG low  = charging
STDBY low = full
USB_DET is not used in v1.0.0-rc1
```

---

## 5. ADC Battery Measurement

Battery voltage is measured through PA0.

Hardware divider:

```text
BAT+ ---- R_UP ---- PA0 ---- R_DOWN ---- GND
```

Current measured values:

| Point | Voltage      |
| ----- | ------------ |
| BAT+  | About 3.92 V |
| PA0   | About 1.94 V |

The divider ratio is approximately 1:1.

Firmware setting:

```c
#define BAT_R_UP_KOHM      100
#define BAT_R_DOWN_KOHM    100
```

Battery voltage formula:

```c
BAT_mV = ADC_mV * (R_UP + R_DOWN) / R_DOWN
```

For the current 100k / 100k divider:

```c
BAT_mV = ADC_mV * 2
```

---

## 6. Reserved / Not Used Resources

| Resource         | Status   | Reason                             |
| ---------------- | -------- | ---------------------------------- |
| LED4 / PA9       | Not used | PA9 is reserved for UART TX        |
| USB_DET / PA10   | Not used | PA10 is reserved for UART RX       |
| External crystal | Not used | Firmware uses internal IRC8M clock |

---

## 7. Current Stable Version

Current stable tag:

```text
v1.0.0-rc1
```

Verified features:

1. PWM backlight control
2. Key brightness adjustment
3. UART command system
4. Flash brightness save and restore
5. ADC battery voltage measurement
6. Battery percentage calculation
7. 3-LED battery display
8. CHRG / STDBY charging status detection
9. Charging LED animation

---

## 8. Important Firmware Constraints

The following constraints must be followed when modifying the firmware:

1. PA6 is used for PWM output to TPS61165.
2. PA6 must not be used as a battery LED.
3. PA9 is reserved for USART0 TX.
4. PA9 must not be configured as LED4 in debug firmware.
5. PA10 is reserved for USART0 RX.
6. USB_DET is not enabled in the current debug firmware.
7. Battery display uses only LED1, LED2 and LED3.
8. CHRG and STDBY are used to detect charging status.
9. The system clock uses internal IRC8M.
10. The current stable baseline is v1.0.0-rc1.

---

## 9. Next Hardware / Firmware Options

Future options:

1. If LED4 is required, UART debug must be disabled or moved to another interface.
2. If USB_DET is required, UART RX on PA10 must be disabled or moved.
3. If full 4-LED battery display is required, the pin map must be redesigned.
4. If production firmware does not need UART commands, PA9 and PA10 can be reused.
5. If debug firmware still needs UART, keep the current pin assignment unchanged.
