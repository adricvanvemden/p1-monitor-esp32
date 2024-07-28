#include <WiFi.h>
#include <TelnetStream.h>
#include "telnet_handler.h"

void setupTelnet() {
  TelnetStream.begin();
  TelnetStream.println("Telnet server started");
}

void handleTelnet() {
  if (TelnetStream.available()) {
    while (TelnetStream.available()) {
      Serial.write(TelnetStream.read());
    }
  }

  if (Serial.available()) {
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    TelnetStream.write(sbuf, len);
  }
}