#include "dsmr_parser.h"

// Data variables
String meterID;                     // Meter ID
String timestamp;                  // Timestamp of the measurement
float totalConsumptionTariff1;     // Total consumption for tariff 1
float totalConsumptionTariff2;     // Total consumption for tariff 2
float totalProductionTariff1;      // Total production for tariff 1
float totalProductionTariff2;      // Total production for tariff 2
int currentTariff;                 // Current tariff
float currentConsumption;          // Current consumption in kW
float currentProduction;           // Current production in kW
int powerFailures;                 // Number of power failures
int longPowerFailures;             // Number of long power failures
int voltageSagsPhaseL1;            // Number of voltage sags in phase L1
int voltageSwellPhaseL1;           // Number of voltage swells in phase L1
float currentL1;                   // Current in phase L1 in A
float activePowerL1Consumption;   // Active power consumption in phase L1 in kW
float activePowerL1Production;    // Active power production in phase L1 in kW
String gasMeterID;                 // Gas meter ID
float gasMeterReading;             // Gas meter reading in m³
String gasMeterTimestamp;             // Gas meter reading in m³

void parseLine(const String line) {
  int startPos, endPos;

  if (line.startsWith("0-0:1.0.0")) {
    // Extract timestamp between parentheses
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    timestamp = line.substring(startPos, endPos);
  } 
  else if (line.startsWith("0-0:96.1.1")) {
    // Extract meter ID between parentheses
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    meterID = line.substring(startPos, endPos);
  } 
  else if (line.startsWith("1-0:1.8.1")) {
    // Extract total consumption for tariff 1 between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    totalConsumptionTariff1 = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("1-0:1.8.2")) {
    // Extract total consumption for tariff 2 between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    totalConsumptionTariff2 = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("1-0:2.8.1")) {
    // Extract total production for tariff 1 between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    totalProductionTariff1 = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("1-0:2.8.2")) {
    // Extract total production for tariff 2 between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    totalProductionTariff2 = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("0-0:96.14.0")) {
    // Extract current tariff between parentheses and convert to integer
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('S');
    currentTariff = line.substring(startPos, endPos).toInt();
  } 
  else if (line.startsWith("1-0:1.7.0")) {
    // Extract current consumption in kW between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    currentConsumption = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("1-0:2.7.0")) {
    // Extract current production in kW between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    currentProduction = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("0-0:96.7.21")) {
    // Extract number of power failures between parentheses and convert to integer
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    powerFailures = line.substring(startPos, endPos).toInt();
  } 
  else if (line.startsWith("0-0:96.7.9")) {
    // Extract number of long power failures between parentheses and convert to integer
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    longPowerFailures = line.substring(startPos, endPos).toInt();
  } 
  else if (line.startsWith("1-0:32.32.0")) {
    // Extract number of voltage sags in phase L1 between parentheses and convert to integer
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    voltageSagsPhaseL1 = line.substring(startPos, endPos).toInt();
  } 
  else if (line.startsWith("1-0:32.36.0")) {
    // Extract number of voltage swells in phase L1 between parentheses and convert to integer
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    voltageSwellPhaseL1 = line.substring(startPos, endPos).toInt();
  } 
  else if (line.startsWith("1-0:31.7.0")) {
    // Extract current in phase L1 in amperes between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    currentL1 = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("1-0:21.7.0")) {
    // Extract active power consumption in phase L1 in kilowatts between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    activePowerL1Consumption = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("1-0:22.7.0")) {
    // Extract active power production in phase L1 in kilowatts between parentheses and convert to float
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('*');
    activePowerL1Production = line.substring(startPos, endPos).toFloat();
  } 
  else if (line.startsWith("0-1:96.1.0")) {
    // Extract gas meter ID between parentheses
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf(')');
    gasMeterID = line.substring(startPos, endPos);
  } 
  else if (line.startsWith("0-1:24.2.1")) {
    // Extract timestamp between the first set of parentheses
    startPos = line.indexOf('(') + 1;
    endPos = line.indexOf('S');
    gasMeterTimestamp = line.substring(startPos, endPos);

    // Extract gas meter reading between the second set of parentheses and convert to double
    startPos = line.indexOf('(', endPos) + 1;
    endPos = line.indexOf('*', startPos);
    gasMeterReading = line.substring(startPos, endPos).toFloat();
  }
}
