# Motor PID Controller

A proportional–integral–derivative (PID) control system for precise motor speed and position regulation.
This project demonstrates closed-loop control using feedback from a sensor to dynamically adjust the motor output and maintain stability.

## Hardware

- Microcontroller: Raspberry Pi Pico
- Motor Driver: L298N dual H-bridge driver
- Motor: DC motor with encoder feedback
- Power Supply: 12 V DC
- Sensors: Rotary encoder for RPM feedback
- Additional Components: Breadboard, jumper wires, USB cable

## Software

The control firmware is written in C/C++ (Arduino-Pico SDK).
It implements a classic PID loop with tunable parameters and serial monitoring.
