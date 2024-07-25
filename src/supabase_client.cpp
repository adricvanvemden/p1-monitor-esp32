#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "dsmr_parser.h"

String formatTimestamp(const String& rawTimestamp) {
  // Assuming rawTimestamp is in the format YYMMDDhhmm
  // Convert it to YYYY-MM-DD HH:MM:SS
  String year = "20" + rawTimestamp.substring(0, 2); // Add "20" to the year part
  String month = rawTimestamp.substring(2, 4);
  String day = rawTimestamp.substring(4, 6);
  String hour = rawTimestamp.substring(6, 8);
  String minute = rawTimestamp.substring(8, 10);
  String second = rawTimestamp.substring(10, 12);

  String formattedTimestamp = year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;
  return formattedTimestamp;
}

void sendDataToSupabase() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(supabase_url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", supabase_key);

    String formattedTimestamp = formatTimestamp(timestamp);

    JsonDocument doc;
    doc["meter_id"] = meterID;
    doc["timestamp"] = formattedTimestamp;
    doc["total_consumption_tariff_1"] = totalConsumptionTariff1;
    doc["total_consumption_tariff_2"] = totalConsumptionTariff2;
    doc["total_production_tariff_1"] = totalProductionTariff1;
    doc["total_production_tariff_2"] = totalProductionTariff2;
    doc["current_tariff"] = currentTariff;
    doc["current_consumption"] = currentConsumption;
    doc["current_production"] = currentProduction;
    doc["power_failures"] = powerFailures;
    doc["long_power_failures"] = longPowerFailures;
    doc["voltage_sags_phase_l1"] = voltageSagsPhaseL1;
    doc["voltage_swell_phase_l1"] = voltageSwellPhaseL1;
    doc["current_l1"] = currentL1;
    doc["active_power_l1_consumption"] = activePowerL1Consumption;
    doc["active_power_l1_production"] = activePowerL1Production;
    doc["gas_meter_id"] = gasMeterID;
    doc["gas_meter_reading"] = gasMeterReading;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
