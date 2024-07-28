#include <WiFi.h>
#include <TelnetStream.h>
#include "telnet_handler.h"
#include <ESPmDNS.h>
#include "config.h"

void SerialTelnetPrint(const String &message) {
  Serial.print(message);
  TelnetStream.print(message);
}

void SerialTelnetPrintln(const String &message) {
  Serial.println(message);
  TelnetStream.println(message);
}

void SerialTelnetPrintF(const char* format, ...) {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
    TelnetStream.print(buffer);
}

void setupTelnet() {
  TelnetStream.begin();
  TelnetStream.println("Telnet server started");

    // Initialize mDNS
  if (!MDNS.begin(hostname)) {
    SerialTelnetPrintln("Error setting up MDNS responder!");
  } else {
    SerialTelnetPrintln("mDNS responder started");
  }
}

void handleTelnet() {
    if (TelnetStream.available()) {
      Serial.write(TelnetStream.read());
    }
  

  if (Serial.available()) {
    size_t len = Serial.available();
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    TelnetStream.write(sbuf, len);
  }
}