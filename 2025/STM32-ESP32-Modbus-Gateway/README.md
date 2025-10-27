# STM32–ESP32 Modbus Gateway (FreeRTOS + Web Interface)

A dual-MCU system combining **STM32** and **ESP32**, designed to demonstrate  
real-time communication over **Modbus RTU** and live web-based telemetry.  
The ESP32 acts as a **gateway and web server**, while the STM32 operates as a **Modbus device**.  
The project integrates **FreeRTOS**, **queues**, and **semaphores** to ensure deterministic data flow.

---

## Features

- **Modbus RTU communication** between STM32 (slave) and ESP32 (master)
- **Web dashboard** hosted on ESP32 for real-time monitoring and control  
- **LED control** and data visualization directly from the web page  
- **FreeRTOS-based task scheduling** with inter-task communication via queues and semaphores  
- **Structured modular code** for both STM32 (HAL) and ESP32 (ESP-IDF/Arduino-RTOS)  
- **Scalable design** – easy to extend with sensors, actuators, or additional Modbus registers  
