#include <ArduinoOTA.h>
#include <WiFi.h>
#include "OTA.h"
#include "config.h"
#include "telnet_handler.h"

void setupOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    SerialTelnetPrintln("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    SerialTelnetPrintln("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    SerialTelnetPrintF("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    SerialTelnetPrintF("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      SerialTelnetPrintln("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      SerialTelnetPrintln("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      SerialTelnetPrintln("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      SerialTelnetPrintln("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      SerialTelnetPrintln("End Failed");
    }
  });

  ArduinoOTA.begin();
}

void handleOTA() {
  ArduinoOTA.handle();
}