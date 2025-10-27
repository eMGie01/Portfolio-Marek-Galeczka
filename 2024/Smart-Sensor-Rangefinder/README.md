# Smart Sensor Rangefinder

A compact intelligent distance measurement device designed to measure and calculate room dimensions using a LiDAR sensor and an ESP32 microcontroller.
Developed as part of a university engineering project to provide an accurate, user-friendly tool for architects and interior designers.
###
The system measures distances in the range of 0.2 m – 8.0 m, calculates area, and allows the user to switch between different measurement units (centimeters, millimeters, and inches).
It integrates an ESP32 DevKitC, a TF-Luna LiDAR sensor, and a 1.8” TFT LCD for real-time display.

## Hardware

### Microcontroller: ESP32 DevKitC
- Dual-core Tensilica LX6 @ 240 MHz
- 520 kB SRAM, 4 MB Flash
- UART, SPI, I2C interfaces
- Integrated Wi-Fi + Bluetooth
### Distance Sensor: TF-Luna LiDAR
- Range: 0.2–8 m
- Accuracy: ±2%
- Interface: UART / I2C
- Power: 5 V, 70 mA avg.
### Display: Waveshare TFT 1.8” (ST7735S driver, 128×160 RGB, SPI)
### Power Supply: 4 × AA batteries (5.2 V total)
### Buttons:
- MENU – select operation mode
- UNIT – change measurement unit
- START – trigger measurement
### Enclosure: Cardboard housing for protection and portability

## Software

The firmware was written in C/C++ (Arduino framework) for ESP32.
###
Main features:
- Measurement of distance using LiDAR (ToF principle — Time of Flight)
- Calculation of room area in “Area” mode
- Unit switching: cm, mm, inch
- Display update via SPI interface (ST7735S controller)
- Basic menu navigation using hardware buttons
