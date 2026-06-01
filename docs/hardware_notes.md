# Hardware Notes

Project: GD32E230F8 Backlight Controller
MCU: GD32E230F8V6
Version: v1.0.0-rc1

---

## 1. Hardware Overview

This project is a battery-powered backlight controller for a 17PM backlight module.

Main hardware blocks:

1. USB Type-C 5V input
2. TC4056A single-cell lithium battery charger
3. DW01A + 8205A battery protection circuit
4. ME6211 3.3V LDO for MCU power supply
5. GD32E230F8V6 MCU
6. TPS61165 LED boost backlight driver
7. PWM brightness control
8. Key brightness control
9. Battery voltage ADC divider
10. CHRG / STDBY charging status detection
11. Battery level LEDs

---

## 2. Power Architecture

Power flow:

```text
USB Type-C 5V
    ↓
TC4056A battery charger
    ↓
Single-cell lithium battery
    ↓
DW01A + 8205A protection
    ↓
System battery rail
    ↓
ME6211 3.3V LDO
    ↓
GD32E230F8V6 MCU
```

Backlight power flow:

```text
Battery rail
    ↓
TPS61165 boost LED driver
    ↓
17PM backlight module
```

---

## 3. MCU Power Supply

The MCU is powered by ME6211 3.3V LDO.

Verified status:

| Item                 | Status   |
| -------------------- | -------- |
| 3.3V supply          | Verified |
| MCU boot             | Verified |
| SWD download         | Verified |
| Internal IRC8M clock | Used     |
| External crystal     | Not used |

Important note:

```text
The current firmware uses the internal IRC8M clock.
No external crystal is used in the current hardware / firmware configuration.
```

---

## 4. Backlight Driver

The backlight is driven by TPS61165.

TPS61165 is used as a boost constant-current LED driver.

Control method:

```text
MCU PWM output
    ↓
TPS61165 CTRL pin
    ↓
Backlight brightness changes
```

Current verified result:

| Item                    | Status   |
| ----------------------- | -------- |
| TPS61165 boost output   | Verified |
| Backlight can light up  | Verified |
| PWM brightness control  | Verified |
| Key brightness control  | Verified |
| UART brightness command | Verified |

---

## 5. PWM Hardware Change

### 5.1 Original Issue

The original PWM pin was PA1.

However, PA1 was not suitable for the current timer PWM implementation.

### 5.2 Current Solution

The PWM signal was moved to PA6 by fly wire.

Current PWM resource:

| Item          | Value               |
| ------------- | ------------------- |
| PWM Pin       | PA6                 |
| Timer         | TIMER2_CH0          |
| Function      | TPS61165 CTRL input |
| PWM Frequency | 10 kHz              |
| Duty Range    | 0% ~ 100%           |

Current decision:

```text
PA6 = PWM_CTRL
PA6 must not be used as battery LED output.
```

---

## 6. Battery Level LEDs

The current firmware uses 3 battery LEDs.

Current LED mapping:

| LED  | MCU Pin | Status   |
| ---- | ------- | -------- |
| LED1 | PA1     | Used     |
| LED2 | PA7     | Used     |
| LED3 | PB1     | Used     |
| LED4 | PA9     | Not used |

Reason LED4 is not used:

```text
PA9 is reserved for USART0 TX in the debug firmware.
Therefore PA9 must not be used as LED4.
```

Current decision:

```text
Battery display uses 3 LEDs only.
LED4 is disabled in v1.0.0-rc1.
```

---

## 7. UART Debug Interface

USART0 is used as the debug and command interface.

Current UART mapping:

| Signal  | MCU Pin | Usage     |
| ------- | ------- | --------- |
| UART_TX | PA9     | USART0 TX |
| UART_RX | PA10    | USART0 RX |

Purpose:

1. Print system status
2. Print brightness level and duty
3. Print battery voltage and percentage
4. Print charge status
5. Receive command strings
6. Debug firmware behavior

Current decision:

```text
PA9 = UART_TX
PA10 = UART_RX
UART is enabled in the debug firmware.
```

---

## 8. USB_DET Limitation

PA10 is connected to USB_DET in hardware, but PA10 is also used as USART0 RX.

Current decision:

```text
PA10 = UART_RX
USB_DET = Not enabled in debug firmware
```

Reason:

1. UART RX is required for the command system.
2. USB_DET is not required for v1.0.0-rc1.
3. Charging status can be detected through CHRG and STDBY.

Important note:

```text
USB insertion detection is not used in v1.0.0-rc1.
If USB_DET is required in the future, UART RX must be disabled or moved.
```

---

## 9. Charging Status Detection

Charging status is detected through TC4056A CHRG and STDBY pins.

Current mapping:

| Signal | MCU Pin | Active Level | Meaning                   |
| ------ | ------- | ------------ | ------------------------- |
| CHRG   | PA4     | Low          | Battery is charging       |
| STDBY  | PA5     | Low          | Battery is full / standby |

Status logic:

| CHRG | STDBY | Firmware Status |
| ---- | ----- | --------------- |
| Low  | High  | Charging        |
| High | Low   | Full            |
| High | High  | Not charging    |
| Low  | Low   | Unknown         |

Current decision:

```text
Use CHRG and STDBY to detect charging status.
Do not use USB_DET in v1.0.0-rc1.
```

---

## 10. Battery Voltage Measurement

Battery voltage is measured by PA0 ADC.

Hardware divider:

```text
BAT+ ---- R_UP ---- PA0 ---- R_DOWN ---- GND
```

Current divider setting:

```c
#define BAT_R_UP_KOHM      100
#define BAT_R_DOWN_KOHM    100
```

Measured values:

| Point | Voltage      |
| ----- | ------------ |
| BAT+  | About 3.92 V |
| PA0   | About 1.94 V |

The divider ratio is approximately 1:1.

Formula:

```c
ADC_mV = raw * 3300 / 4095
BAT_mV = ADC_mV * (R_UP + R_DOWN) / R_DOWN
```

For the current 100k / 100k divider:

```c
BAT_mV = ADC_mV * 2
```

---

## 11. Battery Percentage Display

The current firmware supports battery voltage measurement and battery percentage calculation.

The battery level is displayed by 3 LEDs.

Recommended rough voltage mapping:

| Battery Voltage | Display     |
| --------------- | ----------- |
| < 3.4V          | Low battery |
| 3.4V ~ 3.6V     | 1 LED       |
| 3.6V ~ 3.8V     | 2 LEDs      |
| 3.8V ~ 4.0V     | 3 LEDs      |
| > 4.0V          | Full        |

Important note:

```text
The voltage-to-percentage mapping is only an approximate estimate.
Actual battery percentage should be adjusted after real battery discharge testing.
```

---

## 12. Known Hardware Conflicts

### 12.1 PA6 Conflict

| Resource | Conflict                                |
| -------- | --------------------------------------- |
| PA6      | Used as PWM_CTRL, cannot be used as LED |

Decision:

```text
PA6 is dedicated to PWM output.
```

---

### 12.2 PA9 Conflict

| Resource | Conflict                    |
| -------- | --------------------------- |
| PA9      | UART_TX conflicts with LED4 |

Decision:

```text
PA9 is reserved for UART_TX.
LED4 is disabled in debug firmware.
```

---

### 12.3 PA10 Conflict

| Resource | Conflict                       |
| -------- | ------------------------------ |
| PA10     | UART_RX conflicts with USB_DET |

Decision:

```text
PA10 is reserved for UART_RX.
USB_DET is disabled in debug firmware.
```

---

## 13. Debug Firmware Hardware Configuration

The current firmware is a debug firmware.

Debug firmware configuration:

| Resource              | Status          |
| --------------------- | --------------- |
| UART TX               | Enabled on PA9  |
| UART RX               | Enabled on PA10 |
| LED4                  | Disabled        |
| USB_DET               | Disabled        |
| PWM                   | Enabled on PA6  |
| Battery ADC           | Enabled on PA0  |
| CHRG / STDBY          | Enabled         |
| 3-LED battery display | Enabled         |

Purpose:

1. Keep UART available for debugging.
2. Keep command system available.
3. Make firmware development easier.
4. Make AI / Codex assisted debugging easier.

---

## 14. Production Firmware Hardware Options

In the future, a production firmware may use a different configuration.

Possible production changes:

1. Disable UART command system.
2. Reuse PA9 as LED4.
3. Reuse PA10 as USB_DET.
4. Enable 4-LED battery display.
5. Enable USB insertion detection.
6. Keep only key-based brightness control.

Important rule:

```text
Debug firmware and production firmware should have separate configuration.
Do not mix debug pin usage and production pin usage without updating pin_map.md.
```

---

## 15. Current Verified Hardware Features

Current stable version:

```text
v1.0.0-rc1
```

Verified features:

1. MCU power supply is normal.
2. MCU can boot and run firmware.
3. SWD download works.
4. Internal IRC8M clock works.
5. UART debug works.
6. PWM backlight control works.
7. Backlight can light up.
8. Key brightness adjustment works.
9. Flash brightness save and restore works.
10. Battery ADC measurement works.
11. Battery percentage calculation works.
12. 3-LED battery display works.
13. CHRG / STDBY charge status detection works.
14. Charging LED animation works.

---

## 16. Hardware Notes for Future Revision

Recommended improvements for the next PCB revision:

1. Assign a proper timer PWM pin for TPS61165 CTRL.
2. Avoid sharing UART pins with LED or USB_DET.
3. Reserve a separate pin for USB_DET.
4. Reserve a separate pin for LED4 if 4-LED display is required.
5. Keep UART debug pins available through test pads.
6. Add clear silkscreen labels for SWD, UART, BAT+, GND and PWM.
7. Confirm TPS61165 layout and current sense resistor placement.
8. Confirm battery ADC divider values.
9. Confirm CHRG / STDBY pull-up configuration.
10. Add test points for BAT+, 3.3V, PWM_CTRL, CHRG, STDBY and GND.

---

## 17. Important Rules

When modifying hardware or firmware, follow these rules:

1. PA6 must remain PWM output in v1.0.0-rc1.
2. PA9 must remain UART TX in debug firmware.
3. PA10 must remain UART RX in debug firmware.
4. LED4 must remain disabled in debug firmware.
5. USB_DET must remain disabled in debug firmware.
6. CHRG and STDBY are used for charge status.
7. Battery display uses only 3 LEDs in the current firmware.
8. Do not change pin usage without updating `pin_map.md`.
9. Do not change hardware assumptions without updating this document.
10. Do not prepare production firmware without confirming the final pin map.
