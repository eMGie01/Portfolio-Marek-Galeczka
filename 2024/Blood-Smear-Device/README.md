# Blood Smear Device

Automated blood smear prototype designed to support medical laboratory diagnostics.
This project was developed as part of my engineering thesis in Automation and Robotics.
It automates one of the most common manual procedures in hematology — preparing a proper blood smear on a glass slide for microscopic examination.

## Hardware

- Controller: Arduino Uno R3 (ATMega328, 16 MHz)
- Motor Driver: A4988 stepper motor driver
- Actuator: NEMA17 stepper motor
- Limit Switch: WK320
- Power Supply: 12V DC
- Mechanical Frame: Aluminum V-slot profiles + linear bearings
- Custom Components: 3D printed holders for glass slides and bearings (modeled in Autodesk Inventor)

## Software

The control software is written in C/C++ (Arduino IDE).
It implements two main procedures:
- Calibration mode — positions the mechanism using a limit switch.
- Work mode — executes the smear process by controlling stepper motion and simulating hand oscillations.
