# Resource Allocation System using Banker's Algorithm on Arduino

An interactive, hardware-driven demonstration of the **Banker's Algorithm** for deadlock avoidance, built on an Arduino with an OLED display, LEDs, and a buzzer. Users configure resources, watch processes run, and then must select the correct safe sequence to complete execution — all guided by real-time visual and audio feedback.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [Pin Configuration](#pin-configuration)
- [How It Works](#how-it-works)
- [State Machine](#state-machine)
- [Scenarios](#scenarios)
- [Controls](#controls)
- [Libraries](#libraries)
- [Setup & Upload](#setup--upload)

---

## Overview

The Banker's Algorithm is a classic OS-level deadlock avoidance strategy. This project turns it into a hands-on, physical experience:

1. The user selects how many units of **Resource 1 (R1)** and **Resource 2 (R2)** are available (1 or 2 each).
2. Three processes (P0, P1, P2) are run in a fixed order, each claiming resources — shown live on LEDs.
3. The system computes a **safe sequence** using the Banker's Algorithm.
4. The user must then reproduce that safe sequence by selecting processes one at a time. Correct choices are rewarded with a beep; wrong ones trigger an error buzz.

---

## Hardware Requirements

| Component | Qty |
|---|---|
| Arduino (Uno / Nano / compatible) | 1 |
| SSD1306 OLED Display (128×64, I²C) | 1 |
| LEDs | 4 |
| Momentary push buttons | 5 |
| Buzzer (active or passive) | 1 |
| Resistors (as needed for LEDs/buttons) | — |
| Wires & breadboard / PCB | — |

---

## Pin Configuration

| Pin | Assignment |
|---|---|
| 2 | Button — Toggle R1 |
| 3 | Button — Toggle R2 |
| 4 | Button — Cycle Process Selection |
| 5 | Button — OK / Confirm |
| 6 | Button — Reset |
| 8 | LED — R1, Unit A |
| 9 | LED — R1, Unit B |
| 10 | LED — R2, Unit A |
| 11 | LED — R2, Unit B |
| 12 | Buzzer |

All buttons use internal `INPUT_PULLUP` (active LOW). The OLED display connects via I²C (`Wire`) at address `0x3C`.

---

## How It Works

### Resource & Process Model

The system models **2 resource types** (R1 and R2), each with either 1 or 2 available units, and **3 processes** (P0, P1, P2). Each scenario has a pre-defined allocation state that determines which resources each process holds and needs.

### LED Mapping

The four LEDs represent the allocation state of resources across processes:

| LED | Represents |
|---|---|
| LED_R1A (pin 8) | R1, Unit A |
| LED_R1B (pin 9) | R1, Unit B |
| LED_R2A (pin 10) | R2, Unit A |
| LED_R2B (pin 11) | R2, Unit B |

LEDs light up or turn off as processes claim and release resources during each phase.

### Safe Sequence Calculation

For each of the 4 possible resource configurations, a safe sequence is pre-computed and stored in the `safeSeq` table. The system selects the correct sequence based on the chosen scenario via `getScenario()`.

---

## State Machine

The firmware runs as a **5-state finite state machine**:

```
CHOOSE → RUN_ORDER → SHOW_SAFE → EXECUTE → FINISHED
                                     ↑           |
                                     └───────────┘
                                    (loops until correct)
```

| State | Description |
|---|---|
| `CHOOSE` | User toggles R1 and R2 between 1 and 2 units. LEDs reflect the current full allocation. Press OK to proceed. |
| `RUN_ORDER` | Processes P0, P1, P2 are run one at a time. Each press of OK advances to the next process; LEDs update before and after each run. |
| `SHOW_SAFE` | The computed safe sequence is displayed on the OLED for 2 seconds. |
| `EXECUTE` | The user must select processes in safe-sequence order using the process cycle button and OK. Correct → advance; incorrect → error buzz. |
| `FINISHED` | All processes completed successfully. A success message is shown and LEDs stay on. |

The **Reset** button returns to `CHOOSE` from any state at any time.

---

## Scenarios

Toggling R1 and R2 produces **4 distinct scenarios**, each with its own allocation state and safe sequence:

| Scenario | R1 | R2 | Safe Sequence |
|---|---|---|---|
| 0 | 2 | 2 | P1 → P0 → P2 |
| 1 | 2 | 1 | P2 → P0 → P1 |
| 2 | 1 | 2 | P1 → P2 → P0 |
| 3 | 1 | 1 | P0 → P1 → P2 |

---

## Controls

| Button | Action |
|---|---|
| **R1** | Toggle R1 units between 1 and 2 (CHOOSE state only) |
| **R2** | Toggle R2 units between 1 and 2 (CHOOSE state only) |
| **PROC** | Cycle selected process (P0 → P1 → P2 → P0…) during EXECUTE |
| **OK** | Confirm current action / advance state |
| **RESET** | Return to CHOOSE and reset all state |

---

## Libraries

| Library | Purpose |
|---|---|
| `Adafruit_GFX` | Core graphics engine for the OLED |
| `Adafruit_SSD1306` | Driver for the SSD1306 OLED display |
| `Wire` | I²C communication |

Install the Adafruit libraries:
- **Adafruit GFX Library**
- **Adafruit SSD1306**

---

## Setup & Upload

1. **Wire** the hardware according to the [Pin Configuration](#pin-configuration) table.
2. **Install** the required libraries.
3. **Open** `main.cpp`.
4. **Select** your board and port.
5. **Upload** to the Arduino.
6. On power-up, a 2-second splash screen appears, then the system enters the `CHOOSE` state.

---

All logic resides in a single file for straightforward deployment to any Arduino-compatible board.