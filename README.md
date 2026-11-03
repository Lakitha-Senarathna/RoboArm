# RoboArm

A compact AVR-based robotic arm control project built around the ATmega328P microcontroller. This project reads four analog inputs (mapped to potentiometers or other analog control sources), converts them into servo pulse widths, and drives four servos to move a robotic arm in response to the input signals.

The firmware is written in C and is designed for use with an ATmega328P running at 8 MHz. It uses a combination of ADC reading, timer-driven pulse generation, and interrupt-based control to create precise servo pulses.

---

## Project Overview

This repository contains firmware for a 4-servo robotic arm. The code is designed to:

- Read analog values from four ADC channels
- Scale each analog value into a servo control pulse range
- Update servo angle positions continuously in real time
- Drive four servos using timer and interrupt logic
- Compile and flash directly to an ATmega328P using AVR tools

The project is a prototype/experimental embedded control system intended to demonstrate low-level servo control and embedded timing techniques.

---

## Features

- ATmega328P microcontroller firmware
- 4 independent servo control channels
- 4 analog input channels for control
- ADC-based mapping from input signal to servo pulse width
- Timer-driven servo pulse generation
- Interrupt-driven signal timing for precise output pulses
- Build system using `avr-gcc` and `make`
- Support for flashing the MCU using `avrdude`
- Ready-to-use `.hex` build artifacts included in the repo

---

## Hardware Concept

The project is designed around a small robotic arm with four servos:

- Servo 1
- Servo 2
- Servo 3
- Servo 4

The firmware uses:
- ADC0, ADC1, ADC2, ADC3 as the four analog inputs
- Timer0 and Timer2 for manually timed servo pulses
- Timer1 for hardware PWM generation on two servo outputs

The control logic maps ADC readings to pulse widths between approximately 1000 µs and 2000 µs, which is the standard range used by many hobby servos.

---

## Pin Mapping Used in Firmware

The code defines the following servo output pins:

| Servo | MCU Pin | Purpose |
|-------|---------|---------|
| Servo 1 | PD0 | Output signal line |
| Servo 2 | PD1 | Output signal line |
| Servo 3 | PB1 | Output signal line |
| Servo 4 | PB2 | Output signal line |

The ADC channels are defined as:

| ADC Channel | Input |
|------------|-------|
| ADC0 | Joint / Servo 1 control |
| ADC1 | Joint / Servo 2 control |
| ADC2 | Joint / Servo 3 control |
| ADC3 | Joint / Servo 4 control |

These are used in the main loop to continually read the analog signal values and update the servo positions.

---

## Firmware Architecture

The firmware is organized around a few core functions:

### 1. ADC Setup
The `setADC()` function configures the ADC peripheral using:
- AVCC as the ADC reference (`REFS0`)
- ADC prescaler settings
- ADC enable bit

This is essential for reading analog values from the potentiometer or sensor inputs.

### 2. Timer Setup
The firmware configures several timers:

- `setTimer0()`
- `setTimer1()`
- `setTimer2()`

These timers are used to generate timing signals for:
- Servo pulse widths
- Interrupt-driven pulse shaping
- Servo position updates

### 3. Analog-to-Servo Conversion
The project contains functions such as:

- `adcMapping_servo1()`
- `adcMapping_servo2()`
- `adcMapping_servo3()`
- `adcMapping_servo4()`

Each function converts an ADC reading into a pulse length in microseconds. The code maps the 10-bit ADC value (0–1023) to a servo pulse range around 1000–2000 µs.

### 4. Main Control Loop
In `main()`, the firmware:

1. Configures all servo pins as outputs
2. Initializes ADC and timers
3. Sets initial pulse widths
4. Enables interrupts
5. Continuously reads all four ADC channels
6. Updates servo output values
7. Keeps the robotic arm synchronized with the analog inputs

### 5. Interrupt Service Routines
The project uses interrupt service routines:

- `ISR(TIMER0_COMPA_vect)`
- `ISR(TIMER2_COMPA_vect)`

These interrupts handle the precise creation of servo pulses for the first two servos. The code uses state variables to create the necessary high and low timing windows for pulse generation.

This is a classic embedded technique where the CPU uses timers to generate pulse widths without software blocking.

---

## Important Technical Details

### Clock Frequency
The project defines:

```c
#define F_CPU 8000000UL
```

This indicates the MCU runs at 8 MHz. This is important because all timer calculations and pulse timing depend on that frequency.

### Servo Timing
The firmware builds servo pulses to approximately:
- Minimum pulse: ~1000 µs
- Maximum pulse: ~2000 µs
- Signal repetition period: ~20 ms

This matches standard hobby servo behavior.

### ADC Resolution
The ADC is 10-bit:

```c
return ADC;
```

with values from 0 to 1023. The firmware then scales the reading to a pulse width based on:

```c
1000 + ((uint32_t)adcValue * 1000) / 1023;
```

This produces output values that correspond to servo angles.

---

## Build and Flash Instructions

This repository includes a Makefile for building the firmware. The code is intended to be compiled for an AVR target using `avr-gcc`.

### Prerequisites

You need the AVR toolchain installed:

- `avr-gcc`
- `avr-objcopy`
- `avr-size`
- `avrdude`

On Linux/macOS, these are typically installed with:
- `avr-libc`
- `binutils-avr`
- `gcc-avr`
- `avrdude`

### Build
From the project directory:

```bash
cd RoboArm
make
```

This should produce:
- `RoboArm.elf`
- `RoboArm.hex`
- `RoboArm.map`

### Flash to the Device
The Makefile supports several programmer configurations through `avrdude`. The default configuration is set to:

```make
PROGRAMMER_TYPE = USBasp
```

To flash the firmware:

```bash
make flash
```

or specifically:

```bash
make flash_usbasp
```

You can also use:

```bash
make flash_arduinoISP
```

or other target variants defined in the Makefile depending on your programmer.

### Clean Build Artifacts
To remove generated binaries:

```bash
make clean
```

To remove all compiled outputs (more aggressive cleanup):

```bash
make squeaky_clean
```

---

## Makefile Notes

The `Makefile` is based on a standard AVR project template and includes:

- MCU selection:
  ```make
  MCU = atmega328p
  ```
- Clock:
  ```make
  F_CPU = 8000000UL
  ```
- Programmer defaults:
  ```make
  PROGRAMMER_TYPE = USBasp
  ```
- Fuse settings and flashing commands
- Support for `size`, `disassemble`, `flash`, and `program`

This is useful for quick development and deployment onto an ATmega328P.

---

## Repository Structure

The repository currently contains:

```text
RoboArm/
├── Makefile
├── secondattempt.c
├── RoboArm.elf
├── RoboArm.hex
├── RoboArm.map
├── Test.elf
├── Test.hex
├── Test.map
├── secondattempt.o
```

### File Descriptions

- `secondattempt.c`  
  Main firmware source file implementing the robotic arm control logic.

- `Makefile`  
  Build and flash automation for AVR compilation.

- `RoboArm.hex`  
  Compiled firmware in Intel HEX format.

- `RoboArm.elf`  
  ELF executable image for the ATmega328P.

- `RoboArm.map`  
  Memory map file showing memory allocations.

- `Test.*` files  
  Alternative compile/test outputs; likely used during development or debugging.

---

## How the Control Logic Works

The software performs the following steps:

1. Initializes the servo output pins.
2. Configures ADC and timers.
3. Sets a default pulse width for each servo.
4. Starts interrupts.
5. In the infinite loop:
   - Reads four ADC values
   - Maps each to a servo pulse width
   - Updates the servo PWM registers
6. Timer interrupts keep the servo pulses timed correctly

This combination allows the servo signals to maintain their proper pulse widths while the control values are updated dynamically.

---

## Potential Issues / Notes

This is a prototype firmware and may require tuning depending on:

- Servo model
- Power supply current capability
- Potentiometer wiring
- ADC reference voltage
- Mechanical arm characteristics

Some practical considerations:

- Servos can draw significant current; use a separate 5V supply if necessary
- Keep grounds common between the MCU and the servo power source
- Use proper decoupling capacitors near the MCU and servo power lines
- Check signal pin compatibility with the servo input logic
- Calibrate the analog input ranges to match the desired arm motion

---

## Suggested Hardware Wiring

A typical setup would include:

- ATmega328P microcontroller
- 4 hobby servos
- 4 analog control potentiometers or sensor inputs
- Common ground between MCU and servo power rail
- External 5V regulator or stable supply for servos

Recommended wiring concept:

- Servo signal wires to AVR output pins:
  - PD0, PD1, PB1, PB2
- Potentiometer outputs to:
  - ADC0, ADC1, ADC2, ADC3
- Power supply:
  - MCU powered from regulated 5V or the board’s 5V rail
  - Servos powered from a sufficiently capable source

---

## Example Use Case

This project can be used as:

- A learning platform for embedded servo control
- A robot arm prototype controller
- An AVR-based control demonstration
- A test project for ADC + timer + interrupt design

It is especially useful for understanding:
- ADC configuration
- PWM and pulse timing
- Servo signal generation
- Real-time microcontroller control systems

---

## Limitations

This code is not a general-purpose robotics framework. It is a focused embedded control program that demonstrates low-level servo control on an AVR microcontroller.

Some limitations include:

- Fixed servo configuration
- Hardcoded channel mapping
- No serial communication interface
- No user interface or menu system
- No safety interlocks or motion limits
- No calibration or persistence settings

These can be added later if the project evolves into a more complete robotic arm controller.

---

## Future Improvements

Possible enhancements include:

- Add serial debugging over UART
- Implement servo smoothing and interpolation
- Add angle limits to prevent mechanical collisions
- Support joystick input instead of potentiometers
- Add state machine for motion sequences
- Add user calibration routines
- Add EEPROM storage for default positions
- Add support for more than four joints
- Add a proper PCB design or schematic

---

## Summary

The `RoboArm` project is a practical embedded system implementation for controlling a 4-servo robotic arm using an ATmega328P. It demonstrates:

- ADC-based signal acquisition
- Timer-driven servo control
- Interrupt-based pulse timing
- Real-time signal processing
- AVR firmware engineering

This repository is ideal for educational use, experimentation, and embedded systems prototyping.

---

## License

This project does not appear to include an explicit license file in the repository snapshot examined here. If you plan to publish or distribute the project publicly, it is recommended to add an appropriate open-source license such as MIT or GPL.

---

## Credits

This project was created as an AVR microcontroller robotic arm control experiment.

---

## Build Command Summary

```bash
cd RoboArm
make
make flash_usbasp
```

If you are using a different programmer, adjust the target in the Makefile accordingly.

---

## Quick Start

```bash
git clone https://github.com/Lakitha-Senarathna/RoboArm.git
cd RoboArm
make
make flash_usbasp
```

Then connect the servo control lines and analog inputs as defined in the firmware, power the system, and observe the arm move according to the ADC inputs.

---

If you want, I can also generate:
1. a more polished GitHub-style README with badges and screenshots section,
2. a shorter version for a repo landing page,
3. or a README tailored specifically for publication on GitHub with project structure and installation instructions.
