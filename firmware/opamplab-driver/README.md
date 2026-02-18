# OpAmpLab130: Custom Silicon Driver & CLI

**Status:** v1.0 (Prototype Verified)
**Target:** ESP32 / SKY130 Reconfigurable OpAmp

## Overview
This component implements the **Hardware Abstraction Layer (HAL)** and **Command Line Interface (CLI)** for the *OpAmpLab130*, a custom mixed-signal IC designed for educational purposes.

The driver manages the 16-bit configuration word via a bit-banged Shift Register protocol, allowing real-time tuning of analog parameters (Bias Current, Miller Compensation) through a Serial terminal.

## Architecture
- **HAL (`opamplab.c/h`):** Encapsulates the physical layer (GPIO manipulation, timing constraints). Uses `unions` and `bit-fields` for efficient register mapping.
- **CLI (`main.cpp`):** Provides a human-readable interface to control the chip without recompiling.

## Register Map
| Bit Range | Field | Description |
| :--- | :--- | :--- |
| [0] | ENABLE | Power ON/OFF control |
| [1] | MODE | 0: Low Noise, 1: High Speed |
| [4:2] | BIAS | Programmable Tail Current (8 levels) |
| [8:5] | MILLER | Frequency Compensation Cap (16 levels) |

## Usage
Connect the ESP32 to the OpAmpLab130 digital interface:
- **DATA:** GPIO 23
- **CLK:** GPIO 18
- **LATCH:** GPIO 5

### CLI Commands
```bash
> set_bias 7    # Sets max bias current
> set_miller 0  # Disables compensation
> enable 1      # Power ON
> status        # Dumps current register state (Hex)