#include <HardwareSerial.h>
#include <WiFi.h>
#include <driver/uart.h>
#include "dsmr_parser.h"
#include "supabase_client.h"
#include "config.h"
#include "wifi_manager.h"
#include "OTA.h"
#include "telnet_handler.h"


// Create an instance of HardwareSerial for UART2
HardwareSerial p1Serial(2); // Use UART2

void setup() {
  Serial.begin(monitorBaudRate);

  // Connect to WiFi
  connectToWiFi();

  // Initialize UART2 with correct pins and baud rate
  p1Serial.begin(p1BaudRate, SERIAL_8N1, RX2, TX2);

  // Configure UART to invert RX signal
  uart_config_t uart_config = {
      .baud_rate = p1BaudRate,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .rx_flow_ctrl_thresh = 122,
      .use_ref_tick = false,
  };
  uart_param_config(UART_NUM_2, &uart_config);
  uart_set_pin(UART_NUM_2, TX2, RX2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_line_inverse(UART_NUM_2, UART_SIGNAL_RXD_INV);

  SerialTelnetPrintln("P1 Port Reader Started");

  setupOTA();
  setupTelnet();
  logIngestConfig();
}

void loop() {
  checkWiFiConnection();
  handleOTA();
  handleTelnet();

  static String inputString = "";
  static bool stringComplete = false;

  while (p1Serial.available()) {
    char inChar = (char)p1Serial.read();
    inputString += inChar;
    if (inChar == '!') { // End of DSMR message indicated by '!'
      stringComplete = true;
    }
  }

  // When the string is complete, process it and reset the variables
  if (stringComplete) {
    SerialTelnetPrintln("Received DSMR data:");
    SerialTelnetPrintln(inputString);

    /*
     * Still parsed on-device, even though the server now does the parsing that counts.
     *
     * The upload sends the raw telegram and `p1-ingest` maps the OBIS codes itself, so these globals
     * no longer leave the device. They stay because this is the only way to tell — over telnet, with
     * no database round-trip — whether the P1 wiring actually works. A telegram that arrives but
     * parses to zeroes points at the meter or the cable, not at the network.
     */
    int startIndex = 0;
    int endIndex = inputString.indexOf('\n');
    while (endIndex != -1) {
      String line = inputString.substring(startIndex, endIndex);
      parseLine(line);
      startIndex = endIndex + 1;
      endIndex = inputString.indexOf('\n', startIndex);
    }

    SerialTelnetPrint("Parsed: ");
    SerialTelnetPrint(String(currentConsumption, 3));
    SerialTelnetPrint(" kW in, ");
    SerialTelnetPrint(String(currentProduction, 3));
    SerialTelnetPrint(" kW out, tariff ");
    SerialTelnetPrint(String(currentTariff));
    SerialTelnetPrint(", gas ");
    SerialTelnetPrintln(String(gasMeterReading, 3));

    // Send the raw telegram; the server parses and stores it.
    sendDataToSupabase(inputString);

    // Reset the variables
    inputString = "";
    stringComplete = false;
  }
}
