# STM32–ESP32 Modbus Gateway (FreeRTOS + Web Interface)

A dual-MCU system combining STM32 and ESP32, designed to demonstrate real-time communication over **Modbus RTU** and live web-based telemetry.  
The ESP32 acts as a gateway and web server, while the STM32 operates as a Modbus device.  
The project integrates FreeRTOS, queues, and semaphores to ensure deterministic data flow.

## Features

- Modbus RTU communication between STM32 (slave) and ESP32 (master)
- Web dashboard hosted on ESP32 for real-time monitoring and control  
- FreeRTOS-based task scheduling with inter-task communication via queues and semaphores  

## Hardware Setup

| Board | Function | Connection |
|--------|-----------|-------------|
| STM32 NUCLEO-L467RG | Modbus Slave | UART2 (PA2 = TX, PA3 = RX) |
| ESP32 DevKit V1 | Modbus Master + Web Interface | UART2 (TX = GPIO17, RX = GPIO16) |
| Communication | UART | 115200 baud, 8N1 |
| LED | GPIO2 (onboard ESP32 LED) | Controlled via web panel |

## Software

### STM32 (Nucleo-L467RG)
- Implemented using STM32CubeIDE
- Uses HAL drivers for UART communication  
- Handles Modbus slave protocol  
- Responds to register read/write requests from ESP32

### ESP32 (DevKit V1)
- Built with ESP-IDF
- Acts as Modbus master  
- Hosts a lightweight HTTP server with a dynamic dashboard   
- Tasks for handling Modbus, web updates, and LED control  

## Web Interface

The ESP32 hosts a simple, responsive web page that:
- Displays live Modbus data (register values, communication status)
- Allows toggling the LED on STM32 or ESP32
- Runs fully locally (no internet required)


