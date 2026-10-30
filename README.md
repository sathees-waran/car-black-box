# Car Black Box System

[![Embedded C](https://img.shields.io/badge/Language-Embedded%20C-blue)](https://en.wikipedia.org/wiki/Embedded_C)
[![Microcontroller](https://img.shields.io/badge/MCU-PIC16F877A-green)](https://www.microchip.com/)
[![IDE](https://img.shields.io/badge/IDE-MPLAB%20X-orange)](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide)
[![Simulation](https://img.shields.io/badge/Simulation-PICSimLab-purple)](https://sourceforge.net/projects/picsim/)

A PIC16F877A-based embedded data-logging system that records selected vehicle events along with their time and speed. It provides a live dashboard, password-protected menu access, event-log viewing, and UART-based log downloading.

**Controller:** PIC16F877A · **Clock:** 20 MHz · **Language:** Embedded C · **Compiler:** MPLAB XC8

## Table of Contents

1. [Project Overview](#01-project-overview)
2. [Key Features](#02-key-features)
3. [System Architecture](#03-system-architecture)
4. [Hardware and Peripherals](#04-hardware-and-peripherals)
5. [Communication Interfaces](#05-communication-interfaces)
6. [Software and Tools](#06-software-and-tools)
7. [Project Structure](#07-project-structure)
8. [Source File Responsibilities](#08-source-file-responsibilities)
9. [Keypad Configuration](#09-keypad-configuration)
10. [Event Logging and EEPROM Layout](#10-event-logging-and-eeprom-layout)
11. [Password Management](#11-password-management)
12. [Build and Simulation](#12-build-and-simulation)
13. [UART Log Download](#13-uart-log-download)
14. [Author](#14-author)
15. [Project Resources and Downloads](#15-project-resources-and-downloads)

## 01. Project Overview

The Car Black Box is an embedded event-recording project developed using the **PIC16F877A** microcontroller. It captures selected events, associates each event with the current time and speed, and stores the records in external EEPROM.

The user interacts with the system through a digital keypad and a 16×2 character LCD (CLCD). Stored records can be viewed on the LCD or transferred to a PC through UART using a serial terminal such as Tera Term.

The project demonstrates Embedded C programming, peripheral interfacing, I2C and UART communication, ADC conversion, interrupt handling, and state-based application design.

## 02. Key Features

- Live dashboard displaying time, event, and speed.
- Gear-up and gear-down event selection.
- Collision event recording through a keypad input.
- RTC-based event timestamping.
- ADC-based speed input.
- Circular event-log storage in external EEPROM.
- Password-protected menu access.
- Event-log viewing on the CLCD.
- UART-based event-log download to a PC.
- Event-log metadata clearing.
- RTC time setting through the menu.
- Password change with EEPROM persistence.

> **Note:** Collision detection in the current implementation is triggered using a keypad switch. It is not automatic physical impact detection.

## 03. System Architecture

### 3.1 State-based application flow

The application follows a state-based approach. The `operation` variable in the main super loop identifies the active screen or operation. The main loop reads keypad input and dispatches execution to the corresponding application function.

| Operation value | Active screen / function |
|---:|---|
| 0 | Dashboard |
| 1 | Password |
| 2 | Menu |
| 3 | View Log |
| 4 | Download Log |
| 5 | Clear Log |
| 6 | Set Time |
| 7 | Change Password |

### 3.2 Super-loop and interrupt approach

- The main super loop handles keypad input and application-screen execution.
- Timer-related timekeeping or lockout handling is coordinated through the interrupt service routine.
- Peripheral drivers are separated into individual source and header files.
- Application screens and event-management functions are grouped in `dashboard.c`.

## 04. Hardware and Peripherals

| No. | Component / peripheral | Purpose |
|---:|---|---|
| 1 | PIC16F877A | Main microcontroller |
| 2 | 20 MHz oscillator | Microcontroller clock source |
| 3 | 16×2 CLCD | Dashboard, menu, prompts, and event-log display |
| 4 | Digital matrix keypad | User input and screen navigation |
| 5 | Potentiometer / analog input | Speed input to the ADC |
| 6 | DS1307 RTC | Maintains time and provides event timestamps |
| 7 | External EEPROM | Stores event records, log metadata, and password |
| 8 | UART interface | Transfers event logs to a PC |
| 9 | PICGenios board in PICSimLab | Simulation and peripheral-interfacing environment |

## 05. Communication Interfaces

| No. | Interface | Application |
|---:|---|---|
| 1 | I2C | Communication with the DS1307 RTC and external EEPROM |
| 2 | UART | Serial transfer of event logs to a PC |
| 3 | GPIO | LCD control, keypad scanning, and digital input/output |
| 4 | ADC | Reads the analog speed input |

## 06. Software and Tools

| No. | Technology / tool | Usage |
|---:|---|---|
| 1 | Embedded C | Firmware development |
| 2 | MPLAB X IDE | Project development and compilation |
| 3 | MPLAB XC8 | PIC microcontroller C compiler |
| 4 | PIC16F877A | Target microcontroller |
| 5 | PICSimLab | Microcontroller and peripheral simulation |
| 6 | PICGenios | Development-board model used in PICSimLab |
| 7 | Tera Term | Serial terminal for receiving log data |
| 8 | Virtual serial-port utility | Connects the simulator serial interface to a PC terminal |

## 07. Project Structure

```text
CAR_BLACK_BOX-SATHEESWARAN_M.X/
├── adc.c
├── adc.h
├── clcd.c
├── clcd.h
├── dashboard.c
├── digital_keypad.c
├── digital_keypad.h
├── ds1307.c
├── ds1307.h
├── external_eeprom.c
├── external_eeprom.h
├── i2c.c
├── i2c.h
├── isr.c
├── main.c
├── main.h
├── timers.c
├── timers.h
├── uart.c
├── uart.h
├── Makefile
├── build/       # Generated build output
├── debug/       # Generated debug output
├── dist/        # Generated distribution output
└── nbproject/   # MPLAB X project configuration
```

## 08. Source File Responsibilities

| Source file | Responsibility |
|---|---|
| `main.c` | Initializes peripherals and application data; runs the main super loop and dispatches operations. |
| `main.h` | Shared declarations, macros, and common definitions. |
| `adc.c` / `adc.h` | ADC initialization, analog-channel selection, and ADC reading functions. |
| `clcd.c` / `clcd.h` | Low-level 16×2 character LCD operations, commands, and display-position definitions. |
| `digital_keypad.c` / `digital_keypad.h` | Keypad initialization, scanning, and key definitions. |
| `dashboard.c` | Application screens and functions: dashboard, password, menu, event logging, log viewing, log downloading, log clearing, time setting, and password changing. |
| `ds1307.c` / `ds1307.h` | I2C-based DS1307 RTC access, register definitions, and time functions. |
| `i2c.c` / `i2c.h` | I2C bus initialization and byte-level communication routines. |
| `external_eeprom.c` / `external_eeprom.h` | External EEPROM read/write operations and related declarations. |
| `isr.c` | Interrupt service routine and interrupt-related handling. |
| `timers.c` / `timers.h` | Timer initialization, configuration, and related declarations. |
| `uart.c` / `uart.h` | UART initialization and serial transmit/receive functions. |

## 09. Keypad Configuration

The following tables show the keypad assignments for each screen.

### 9.1 Dashboard

| Key / input | Assigned function |
|---|---|
| DKP1 (SW1) | Gear Up |
| DKP2 (SW2) | Gear Down |
| DKP3 (SW3) | Record Collision Detection event |
| DKP4 (SW4) | Enter Password screen |
| Potentiometer 1 | Speed input |

### 9.2 Password Screen

| Key | Assigned function |
|---|---|
| DKP1 (SW1) | Enter digit `1` |
| DKP2 (SW2) | Enter digit `0` |
| DKP4 (SW4) | No function assigned |
| DKP5 (SW5) | Exit Password screen and return to Dashboard |

### 9.3 Menu

| Key | Assigned function |
|---|---|
| DKP1 (SW1) | Enter selected menu option |
| DKP2 (SW2) | Exit Menu and return to Dashboard |
| DKP4 (SW4) | Scroll Up |
| DKP5 (SW5) | Scroll Down |

### 9.4 View Log

| Key | Assigned function |
|---|---|
| DKP1 (SW1) | Scroll Up / Previous Log |
| DKP2 (SW2) | Scroll Down / Next Log |
| DKP5 (SW5) | Exit View Log and return to Menu |

### 9.5 Set Time

| Key | Assigned function |
|---|---|
| DKP1 (SW1) | Increment selected digit |
| DKP2 (SW2) | Change selected field |
| DKP4 (SW4) | Save time and exit to Menu |
| DKP5 (SW5) | Exit without saving and return to Menu |

### 9.6 Change Password

| Key | Assigned function |
|---|---|
| DKP1 (SW1) | Enter digit `1` |
| DKP2 (SW2) | Enter digit `0` |
| DKP4 (SW4) | Save new password and exit |
| DKP5 (SW5) | Discard changes and exit |

## 10. Event Logging and EEPROM Layout

The event log is stored in external EEPROM using a circular-buffer arrangement. When the log is full, a new event replaces the oldest record.

### 10.1 Event record format

Each event record occupies 12 bytes.

| Byte offset | Size | Stored information |
|---:|---:|---|
| 0–7 | 8 bytes | Time in `HH:MM:SS` format |
| 8–9 | 2 bytes | Event code: `ON`, `GN`, `G1`–`G5`, `GR`, or `C ` |
| 10–11 | 2 bytes | Speed represented by two ASCII digits |

### 10.2 EEPROM address allocation

| Address range | Purpose |
|---|---|
| 0–119 | 10 event records × 12 bytes |
| 120 | Log head / next record position |
| 121 | Number of valid log records |
| 122–125 | Four-character password |
| 126 | Password validity marker |

The firmware uses these logical definitions:

```c
#define MAX_LOGS              10
#define LOG_SIZE              12
#define LOG_HEAD_ADDR         120
#define LOG_COUNT_ADDR        121
#define PASSWORD_ADDR         122
#define PASSWORD_MARKER_ADDR  126
#define PASSWORD_MARKER       0xA5
```

The oldest record position is calculated using the circular-buffer head and valid record count. Clearing the log resets the log metadata; it does not erase every event byte in EEPROM.

## 11. Password Management

- The initial password is `1100`.
- The password is stored in external EEPROM.
- A validity marker identifies whether a saved password is available.
- If no valid password has been stored, the firmware initializes the default password.
- The Change Password screen verifies the current password and asks for the new password and its confirmation.
- SW4 saves the new password.
- SW5 discards the pending change and exits.

## 12. Build and Simulation

### 12.1 Development environment

- **IDE:** MPLAB X IDE
- **Compiler:** MPLAB XC8
- **Target MCU:** PIC16F877A
- **Oscillator frequency:** 20 MHz
- **Simulation:** PICSimLab with the PICGenios board

### 12.2 Build the project

1. Install MPLAB X IDE.
2. Install the MPLAB XC8 compiler and verify that MPLAB X detects it.
3. Open the `.X` project folder in MPLAB X.
4. Confirm that the source and header files are included in the project.
5. Verify that the target device is PIC16F877A and the selected toolchain is XC8.
6. Ensure the project clock configuration matches the 20 MHz oscillator.
7. Select **Clean and Build Project**.
8. Check the Output window and resolve any compilation errors.

### 12.3 Run the simulation

1. Open PICSimLab.
2. Select the PICGenios development-board model.
3. Load or configure the PIC16F877A firmware.
4. Configure the simulated peripherals and connections according to the project wiring.
5. Set the simulated clock to 20 MHz.
6. Run the simulation and test the LCD, keypad, analog speed input, RTC, EEPROM, and UART functions.

Peripheral availability and wiring depend on the selected PICSimLab board/model and simulator version. Verify the actual simulation configuration.

## 13. UART Log Download

The project transfers stored event records to a PC through UART.

1. Create a connected virtual COM-port pair using a virtual serial-port utility, if required by the simulator setup.
2. Configure the simulator serial interface to use one end of the virtual COM-port pair.
3. Open Tera Term and select the other end of the pair.
4. Configure the serial parameters to match the firmware UART settings.
5. Select **Download Log** from the project menu.
6. View the received event-log data in the Tera Term terminal window.

The baud rate and other serial parameters in Tera Term must match the firmware and simulator configuration.

## 14. Author

**Satheeswaran M**  
Electronics and Communication Engineering

- GitHub: [sathees-waran](https://github.com/sathees-waran)
- LinkedIn: **[Add your LinkedIn profile URL here]**

## 15. Project Resources and Downloads

| Resource | Purpose | Link |
|---|---|---|
| Project Repository | Car Black Box source code | [GitHub Repository](https://github.com/sathees-waran/car-black-box) |
| MPLAB X IDE | PIC project development | [Download](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide) |
| MPLAB XC8 Compiler | Compile firmware for PIC microcontrollers | [Download](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers) |
| PICSimLab | Microcontroller and peripheral simulation | [Project page](https://sourceforge.net/projects/picsim/) |
| PICSimLab 0.9.2 | Version-specific files | [Download directory](https://sourceforge.net/projects/picsim/files/v0.9.2/) |
| Tera Term | Serial terminal application | [Releases](https://github.com/TeraTermProject/teraterm/releases) |
| com0com | Virtual null-modem / paired COM ports | [Project page](https://sourceforge.net/projects/com0com/) |

**Virtual COM-port note:** com0com can create a pair of connected virtual COM ports. Configure the simulator and Tera Term to use the corresponding ends. Compatibility and driver-signing requirements may vary by Windows version and utility release.
