# Multi-I2C-MotorControl

An STM32F103-based embedded project demonstrating modular I²C communication with multiple peripherals. The application controls a Pololu **Motoron M3H550** motor driver using a single potentiometer connected through an **ADS1115 16-bit ADC**, with all I²C devices managed through a **TCA9548A I²C multiplexer**.

This project serves as both a learning example and a reusable driver for STM32-based motor control applications.

---

## Features

- Modular STM32 HAL-based firmware
- I²C communication with multiple devices
- TCA9548A I²C multiplexer support
- ADS1115 16-bit ADC interface
- Pololu Motoron M3H550 motor controller interface
- Bidirectional motor control using a single potentiometer
- Speed control proportional to potentiometer position
- UART debug output for testing and debugging

---

## Hardware

- STM32F103
- Pololu Motoron M3H550
- ADS1115 16-bit ADC
- TCA9548A I²C Multiplexer
- Potentiometer
- DC Motor

---

## Project Structure

```text
Core/
├── Inc/
├── Src/
└── Drivers/
    ├── Inc/
    ├── Src/
    ├── motoron.c
    ├── motoron.h
    ├── app_config.h
    └── ...
```

The firmware is organized into reusable modules to improve readability, maintainability, and portability across STM32 projects.

---

## Demo

### Photos

> <img width="922" height="154" alt="Uart" src="https://github.com/user-attachments/assets/6cacf951-e26e-4688-a750-7e50d7aef510" />
> <img width="1888" height="723" alt="Uart_reveseDir" src="https://github.com/user-attachments/assets/343b8e63-da94-4f10-8dfb-556ad2ede525" />
> <img width="1905" height="509" alt="Logic_analyzer" src="https://github.com/user-attachments/assets/e6a905e4-f6e0-4584-b8b7-0eb3dc1e7cc1" />

### Video

> https://github.com/user-attachments/assets/74741c95-3f2d-4f3b-8417-a1fee3706ca8


---

## Future Improvements

- Closed-loop motor speed control
- Encoder feedback support
- Multiple motor support
- Additional Motoron configuration commands
- Unit tests for driver functions
