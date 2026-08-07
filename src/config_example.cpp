#include "config.h"

// Copy this file to `src/config.cpp` (gitignored) and fill in your own values.
//
// The three ingest values come from Monkey Suite: Energy -> settings -> Meter -> Register meter.
// The token is readable again later via "Show token", so losing it is not fatal.
const char* ingest_url = "https://your-project-ref.supabase.co/functions/v1/p1-ingest";
const char* meter_id = "p1_xxxxxxxxxx";
const char* device_token = "your_device_token";

const char* wifi_ssid = "your_wifi_ssid";
const char* wifi_password = "your_wifi_password";
const int monitorBaudRate = 9600;
const int p1BaudRate = 115200;
const char* hostname = "esp32-p1-reader";
