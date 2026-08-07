#ifndef CONFIG_H
#define CONFIG_H

// Full URL of the Monkey Suite ingest endpoint, e.g.
//   https://<project-ref>.supabase.co/functions/v1/p1-ingest
// Copy it from the app: Energy -> settings -> Meter.
extern const char* ingest_url;

// Credentials minted by register_meter(). `meter_id` looks like `p1_a1b2c3d4e5` — it is NOT the
// meter's own equipment identifier from the telegram, which is a different value entirely.
extern const char* meter_id;
extern const char* device_token;

extern const char* wifi_ssid;
extern const char* wifi_password;
extern const int monitorBaudRate;
extern const int p1BaudRate;
extern const char* hostname;

#endif
