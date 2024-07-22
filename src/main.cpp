#include <HardwareSerial.h>
#include "dsmr_parser.h"
#include "supabase_client.h"
#include "config.h"
#include <WiFi.h>
#include <driver/uart.h>

// Create an instance of HardwareSerial for UART2
HardwareSerial p1Serial(2); // Use UART2

void setup() {
  Serial.begin(monitorBaudRate);

  // Connect to WiFi
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

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

  Serial.println("P1 Port Reader Started");
}

void loop() {
  static String inputString = "";
  static bool stringComplete = false;

  while (p1Serial.available()) {
    char inChar = (char)p1Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  // When the string is complete, process it and reset the variables
  if (stringComplete) {
    Serial.println("Received DSMR data:");
    Serial.println(inputString);

    // Process each line of the DSMR data
    int startIndex = 0;
    int endIndex = inputString.indexOf('\n');
    while (endIndex != -1) {
      String line = inputString.substring(startIndex, endIndex);
      parseLine(line);
      startIndex = endIndex + 1;
      endIndex = inputString.indexOf('\n', startIndex);
    }

    // Send data to Supabase
    sendDataToSupabase();

    // Reset the variables
    inputString = "";
    stringComplete = false;
  }
}
