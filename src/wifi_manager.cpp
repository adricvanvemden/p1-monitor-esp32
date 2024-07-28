#include <WiFi.h>
#include "wifi_manager.h"
#include "config.h"
#include "telnet_handler.h"

void connectToWiFi() {
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        SerialTelnetPrintln("Connecting to WiFi...");
    }
    SerialTelnetPrintln("Connected to WiFi");
}

void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        SerialTelnetPrintln("WiFi connection lost. Reconnecting...");
        connectToWiFi();
    }
}