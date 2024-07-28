#include <WiFi.h>
#include <TelnetStream.h>
#include "telnet_handler.h"
#include <ESPmDNS.h>
#include "config.h"

void setupTelnet() {
  TelnetStream.begin();
  TelnetStream.println("Telnet server started");

    // Initialize mDNS
  if (!MDNS.begin(dns_name)) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
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