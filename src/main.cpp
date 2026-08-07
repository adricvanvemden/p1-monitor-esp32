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

/*
 * Shortest telegram we will accept. A complete Kaifa MA105 DSMR 4.2 telegram measures ~750 bytes;
 * the damaged ones recovered from the database ran 200-400. 500 sits clearly between the two, so it
 * rejects overruns without being so tight that a meter with fewer optional lines gets refused.
 */
static const size_t MIN_TELEGRAM_BYTES = 500;

void setup() {
  Serial.begin(monitorBaudRate);

  // Connect to WiFi
  connectToWiFi();

  /*
   * Enlarge the RX buffer before opening the port — 256 bytes is not enough to survive a POST.
   *
   * The ESP32 default holds 256 bytes; a DSMR 4.2 telegram from this meter is around 750. The read
   * loop drains it continuously, so that is fine right up until `sendDataToSupabase()` blocks on a TLS
   * handshake. Nothing services the UART for the duration, the buffer wraps, and the telegram comes
   * out with a hole in the middle — which still ends in `!`, so it looks complete and gets stored with
   * half its fields null. Roughly one row in ten arrived that way.
   *
   * 2048 holds two whole telegrams, which covers a slow POST without dropping the one behind it.
   * Must be called before begin(); afterwards it is ignored.
   */
  p1Serial.setRxBufferSize(2048);
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
    // A telegram starts at '/'. Restarting here discards any partial one left over from a dropped or
    // interrupted read, so a fragment can never be glued onto the front of the next telegram.
    if (inChar == '/') inputString = "";
    inputString += inChar;
    if (inChar == '!') { // End of DSMR message indicated by '!'
      stringComplete = true;
    }
  }

  // When the string is complete, process it and reset the variables
  if (stringComplete) {
    /*
     * Drop a telegram that lost bytes in transit rather than uploading it.
     *
     * A truncated telegram still ends in '!', so completeness cannot be judged from the terminator
     * alone — the hole is in the middle. Every field the app cares about sits between the header and
     * the gas line, so a short telegram is a damaged one. Uploading it anyway wrote rows with the
     * counters intact and everything else null, which is worse than no row at all: a gap is visible,
     * a half-filled row silently drags averages around.
     */
    if (inputString.length() < MIN_TELEGRAM_BYTES) {
      SerialTelnetPrint("Dropped a short telegram (");
      SerialTelnetPrint(String(inputString.length()));
      SerialTelnetPrintln(" bytes) — RX buffer overrun, not sending");
      inputString = "";
      stringComplete = false;
      return;
    }

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
