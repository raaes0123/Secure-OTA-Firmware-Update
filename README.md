# Secure OTA Firmware Update for ESP32

![ESP32](https://img.shields.io/badge/Platform-ESP32-blue)
![FreeRTOS](https://img.shields.io/badge/OS-FreeRTOS-green)
![LVGL](https://img.shields.io/badge/GUI-LVGL-orange)

## Overview

This project implements a **secure Over-The-Air (OTA) firmware update mechanism** for ESP32-based embedded systems with an SPI LCD touch display. The system downloads firmware securely over HTTPS, updates progress on a LVGL GUI, and flashes the firmware in chunks for safe updates.

### Features

- Connects to WiFi network (STA mode)
- Secure HTTPS/TLS OTA firmware download
- Chunked firmware download
- LVGL-based GUI showing OTA progress
- SPI-based LCD (ILI9341) with touch controller (XPT2046)
- FreeRTOS-based multitasking
- Safe OTA partitions with rollback capability

---

## Hardware Requirements

| Component         | Interface | ESP32 Pins |
|------------------|-----------|------------|
| ILI9341 LCD       | SPI       | SCLK: 18, MOSI: 23, MISO: 19 |
| XPT2046 Touch     | SPI       | CS: 5, CLK: 25, MISO: 39 |
| Backlight         | GPIO      | 21 |

---

## Software Requirements

- ESP-IDF (v5.x or later recommended)
- LVGL (v8.x)
- FreeRTOS (comes with ESP-IDF)
- Mongoose Embedded Networking Library
- CMake and Ninja build system

## File Structure
```
secure-ota-esp32/
├── main.c # Application entry point
├── lcd.c # LCD & Touch driver with LVGL integration
├── lcd.h # Header for lcd.c
├── ota.c # Secure OTA implementation
├── wifi.c # WiFi initialization and connection
├── components/ # Optional custom components
├── sdkconfig # ESP-IDF configuration file
└── README.md # Project documentation```
```

## Architecture & Flow

### Tasks

1. **Main Task (`app_main`)**: Initializes WiFi, LCD, LVGL, and creates OTA & LVGL tasks.
2. **LVGL Task (`example_lvgl_port_task`)**: Updates GUI, receives OTA progress via FreeRTOS queue.
3. **OTA Task (`ota_update_task`)**: Handles secure OTA download, writes firmware chunks to OTA partition.

### OTA Process

1. Connect to WiFi.
2. Initialize LVGL and display GUI.
3. OTA task waits for notification to start.
4. Connect to HTTPS server with TLS handshake.
5. Send `HEAD` request to get firmware size.
6. Download firmware in 1 KB chunks.
7. Write each chunk to OTA partition.
8. Update LVGL progress meter via FreeRTOS queue.
9. Complete OTA and switch boot partition.
10. Restart ESP32 to new firmware.

## How It Works

- **Chunked OTA:** Downloads firmware in small chunks (1 KB) to avoid blocking system.
- **TLS Security:** Uses CA certificate to verify HTTPS OTA server.
- **FreeRTOS Queue:** OTA task sends progress updates to GUI task safely.
- **SPI LCD & Touch:** ILI9341 panel and XPT2046 touch controller provide real-time user feedback.

## Getting Started

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/secure-ota-esp32.git
cd secure-ota-esp32
```
### 2. Set Up ESP-IDF

## Follow the ESP-IDF Getting Started Guide to install ESP-IDF and configure environment variables.
```bash
export IDF_PATH=~/esp/esp-idf
source $IDF_PATH/export.sh
```
### 3. Configure Project

## Set WiFi credentials in main.c:
```
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASS "YourWiFiPassword"
```

## Optionally, configure LCD pins and OTA server URL in lcd.c and ota.c.

### 4. Build the Project
```bash
idf.py set-target esp32
idf.py menuconfig     # Optional: configure pins, OTA settings
idf.py build
```
### 5. Flash to ESP32
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

## Replace /dev/ttyUSB0 with your ESP32 serial port.

### 6. OTA Update

* Once the system is running, OTA can be triggered via FreeRTOS task notification.

* Firmware downloads securely over HTTPS from the configured server.

* GUI updates show download progress in real-time.

### 7. References

- [ESP-IDF OTA API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/ota.html)
- [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- [LVGL Documentation](https://docs.lvgl.io)
- [Mongoose Embedded Networking Library](https://cesanta.com/mongoose)