# ESP32-S3 Interactive Sculpture: GeoVision v1.0

This project uses an ESP32-S3 microcontroller to create an interactive sculpture that determines its geographic location via satellite (GPS) and displays related historical data on a screen. The sculpture runs on a LilyGo T-Display S3 - ESP32-S3 R8, a versatile device with integrated Wi-Fi, Bluetooth, and a TFT display.

## Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Project Setup](#project-setup)
- [How It Works](#how-it-works)
  - [GPS Location and Historical Data](#gps-location-and-historical-data)
  - [Display Output](#display-output)
- [Code Explanation](#code-explanation)
  - [Data Structure](#data-structure)
  - [GPS and Location Handling](#gps-and-location-handling)
  - [Display Management](#display-management)
- [Customization](#customization)
- [Troubleshooting](#troubleshooting)
- [License](#license)

## Overview

This project aims to blend technology with art by using geographic data to interact with the sculpture’s environment. As the sculpture moves, it identifies nearby historical locations within a specified range, retrieves relevant data, and displays this information on its built-in screen.

## Hardware Requirements

- **LilyGo T-Display S3 - ESP32-S3 R8**: The main microcontroller used for this project.
- **GPS Module**: To acquire satellite data and determine geographic location.
- **Wiring and Power Supply**: For connecting the GPS module to the ESP32-S3 and powering the entire setup.

## Software Requirements

- **Arduino IDE**: For coding and uploading the script to the ESP32-S3.
- **TinyGPS++ Library**: A library for handling GPS data.
- **ArduinoJson Library**: For parsing and handling JSON data.
- **TFT_eSPI Library**: To manage the display on the ESP32-S3.

## Project Setup

1. **Install Libraries**: Ensure the following libraries are installed in your Arduino IDE:

   - `TinyGPS++`
   - `ArduinoJson`
   - `TFT_eSPI`

2. **Hardware Wiring**:

   - Connect the GPS module's RX to GPIO 18 and TX to GPIO 17 on the ESP32-S3.
   - Ensure the display power pin (GPIO 15) is correctly connected to power the display.

3. **Upload the Code**:
   - Open the provided code in the Arduino IDE.
   - Select the correct board settings for the LilyGo T-Display S3 - ESP32-S3.
   - Upload the code to the device.

## How It Works

### GPS Location and Historical Data

The device continuously checks for GPS signals to determine its latitude and longitude. Based on the current location, it compares these coordinates with predefined historical sites. If within a 200-meter range of a recognized location, the corresponding historical data is displayed on the screen.

### Display Output

The device displays relevant historical information alongside images associated with the identified location. If no specific site is found within range, the device defaults to a general overview of the city.

## Code Explanation

### Data Structure

The `HistoricalLocation` struct organizes each site's data, including its coordinates, a description, related historical events, and a corresponding image.

### GPS and Location Handling

The GPS module provides latitude and longitude data, processed by the `TinyGPSPlus` library. The `haversine` function calculates distances between the current location and each historical site to determine if a site is within the 200-meter display radius.

### Display Management

The TFT display is managed by the `TFT_eSPI` library. Depending on the identified location, the display shows the associated image and historical data. Text wrapping is implemented to ensure readability on the small screen.

## Customization

You can add more historical locations by updating the JSON data structure. Ensure each entry includes accurate GPS coordinates, a description, and an image reference.

## Troubleshooting

- **No GPS Data Detected**: If the device reports no GPS data, ensure proper wiring and a clear line of sight to the sky.
- **Inaccurate Location Displayed**: Adjust the `haversine` function or check the GPS coordinates for accuracy.
