# DSMR P1 Monitor using ESP32 Devkit V1 with Kaifa MA105 Support

This project is designed to monitor and log data from a DSMR (Dutch Smart Meter Requirements) compliant smart meter, specifically the Kaifa MA105. The ESP32 Devkit V1 microcontroller is used to read the data from the meter's P1 port and send it to a Supabase database for logging and analysis.

## Features

- **Reads DSMR data**: Captures data such as electricity consumption, production, current tariff, and gas meter readings.
- **WiFi Connectivity**: Connects to a WiFi network to transmit data.
- **Supabase Integration**: Sends the captured data to a Supabase database for storage and further analysis.

## Hardware Requirements

- ESP32 Devkit V1
- DSMR-compatible smart meter (Kaifa MA105)
- RJ11 6P4C or 6P6C cable
- 10k Ω resistor

## Software Requirements

- Arduino IDE or PlatformIO
- ArduinoJson library
- WiFi library

## Usage

1. **Change config details** in config_example.cpp and rename to config.cpp
2. **Upload the code** to your ESP32 using Arduino IDE or PlatformIO.
3. **Connect the ESP32 to your smart meter** using the RJ11 cable.

      ### RJ11 Cable Pin Configuration
      
      | Pin | Function               |
      |-----|------------------------|
      | 1   | NC (Not Connected)     |
      | 2   | RTS (Request to Send)  |
      | 3   | GND (Ground)           |
      | 4   | NC (Not Connected)     |
      | 5   | TxD (Transmit data)    |
      | 6   | NC (Not Connected)     |
      
      ### ESP32 Pin Configuration
      
      | RJ11 Pin | ESP32 Pin | Function       |
      |----------|-----------|----------------|
      | 2        | 3.3V      | Power          |
      | 3        | GND       | Ground         |
      | 5        | RX2       | Data Reception |

**Note:** There is a 10k ohm resistor between RJ11 Pin 2 (RTS) and RJ11 Pin 5 (TxD).
          Above is configuration for a RJ11 6P4C, external power is needed.

4. **Monitor the Serial Output** to ensure the ESP32 is connecting to WiFi and sending data to Supabase.
5. **Check Supabase** to verify the data is being received and stored correctly.

