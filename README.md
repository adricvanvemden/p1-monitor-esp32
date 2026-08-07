# DSMR P1 Monitor using ESP32 Devkit V1 with Kaifa MA105 Support

This project is designed to monitor and log data from a DSMR (Dutch Smart Meter Requirements) compliant smart meter, specifically the Kaifa MA105. The ESP32 Devkit V1 microcontroller is used to read the data from the meter's P1 port and send it to a Supabase backend for logging and analysis.

## Features

- **Reads DSMR data**: Captures data such as electricity consumption, production, current tariff, and gas meter readings.
- **WiFi Connectivity**: Connects to a WiFi network to transmit data.
- **Monkey Suite integration**: POSTs each raw telegram to the `p1-ingest` edge function, which parses and stores it.

## How the data flows

The device sends the **raw DSMR telegram**; the server parses it.

```
P1 port --> ESP32 --> POST { meter_id, device_token, raw } --> p1-ingest --> p1_readings
```

The telegram is stored verbatim in `p1_readings.raw` alongside the parsed columns, so any OBIS field
the server does not extract yet — power failures, voltage sags and swells, the gas meter's own id —
is still captured and becomes queryable by extending the server's parser. That is a redeploy rather
than a re-flash of a device sitting in a meter cupboard, which is the whole reason for the split.

The telegram is still parsed on-device too, but only to print a one-line summary over telnet. It is
how you tell a wiring problem from a network problem without going near the database.

Authentication is the `device_token` in the request body, checked against the `meters` table. There
is no API key header — the edge function is deployed with `--no-verify-jwt` because the ESP32 has no
Supabase user session to present.

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

1. **Register the meter in Monkey Suite** — Energy → settings → Meter → *Register meter*. It mints the
   id and token server-side and shows you three values to copy: **meter id** (`p1_…`), **device
   token**, and the **ingest URL**.

   > The `meter_id` is the registered `p1_…` value — **not** the meter's equipment identifier from the
   > telegram. Using the latter is the usual cause of a `401` from the ingest endpoint.

   > Losing the token is not fatal: household members can read it back with *Show token*.

2. **Copy `src/config_example.cpp` to `src/config.cpp`** and fill in those three values plus your WiFi
   credentials. `src/config.cpp` is gitignored, so your secrets stay out of the repo.
3. **Flash it.** The first flash must go over USB — OTA needs firmware already running on the device
   to receive the update, so a fresh board (or one with the wrong WiFi credentials) is only reachable
   by cable.

   ```sh
   pio run -e usb -t upload     # first flash, and recovery when OTA stops answering
   pio run -t upload            # every flash after that — `ota` is the default env
   ```

   > If the upload does not start, hold **BOOT** while it says `Connecting...` and release once it
   > begins. Most DevKit V1 boards auto-reset and never need this; some clones do.

4. **Connect the ESP32 to your smart meter** using the RJ11 cable.

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

5. **Monitor the output** over serial or telnet (`telnet esp32-p1-reader.local`). Each telegram prints
   the raw text, a `Parsed:` summary line, and the ingest response.
6. **Check the app** — readings appear on the Energy page, which subscribes to `p1_readings` inserts in
   realtime.

### Reading the telnet output

| What you see | What it means |
|---|---|
| No `Received DSMR data` at all | P1 wiring, cable or baud rate — nothing is reaching the ESP32 |
| Telegram arrives, `Parsed:` all zeroes | Data is arriving but the OBIS mapping does not match your meter |
| `Ingest 401` | `meter_id` / `device_token` do not match a registered meter |
| `Ingest 400` | The body was missing a field — check `config.cpp` is filled in |
| `Ingest 200` | Working; the row is in `p1_readings` |

