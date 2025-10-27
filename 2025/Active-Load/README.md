# Active Load for battery testing

An advanced electronic load system designed for testing 12 V batteries in constant current (CC), constant resistance (CR), and constant power (CP) modes.
The device is built around a STM32 NUCLEO-L152RE microcontroller, a MOSFET-based power stage, and a PID controller with temperature compensation, ensuring precise regulation and stable operation across a wide current range.
###
This project was developed as a master’s thesis in Automation and Robotics at the Silesian University of Technology (2025).
The system enables controlled battery discharge up to 14 A, with real-time measurement of current, voltage, temperature, and charge, and provides data logging to an SD card or PC via UART communication.

## Hardware

- Microcontroller: STM32 NUCLEO-L152RE (ARM Cortex-M3, 32-bit, 32 MHz)
- Load Element: IRLZ44N N-MOSFET (110 W, 33 A, logic-level)
- Voltage & Current Measurement: 24-bit MCP3561 sigma-delta ADC
- DAC for control: 16-bit DAC8830
- Voltage Reference: ADR441 (2.5 V precision)
- Cooling System: 12 cm fan + DS18B20 temperature sensors
- Display & Controls: LCD 2×16, rotary encoder, push buttons
- Power Supply: 12 V DC
- Data Storage: microSD card module (SPI)
- Enclosure: Polycarbonate case with cooling vents and front-panel interface

## Software

Developed in C (STM32CubeIDE) using HAL libraries.
The system implements a PID controller with real-time compensation for MOSFET thermal drift, ensuring stable regulation under varying load conditions.

### Main features:
- Three operation modes:
   1. CC — Constant Current
   2. CR — Constant Resistance
   3. CP — Constant Power
- PID control with temperature compensation
- Real-time acquisition of voltage, current, and temperature
- Data logging in .CSV format (time, current, voltage, power, temperature)
- PC communication via UART protocol
- Local LCD interface with encoder navigation

## PC Application

A dedicated C# application (Visual Studio) allows:
- Remote configuration of operating mode and parameters
- Real-time monitoring over UART
- Control of data logging and stop conditions
- Visualization of battery discharge characteristics


