#ifndef DSMR_PARSER_H
#define DSMR_PARSER_H

#include <Arduino.h>

extern String meterID;
extern String timestamp;
extern float totalConsumptionTariff1;
extern float totalConsumptionTariff2;
extern float totalProductionTariff1;
extern float totalProductionTariff2;
extern int currentTariff;
extern float currentConsumption;
extern float currentProduction;
extern int powerFailures;
extern int longPowerFailures;
extern int voltageSagsPhaseL1;
extern int voltageSwellPhaseL1;
extern float currentL1;
extern float activePowerL1Consumption;
extern float activePowerL1Production;
extern String gasMeterID;
extern float gasMeterReading;
extern String gasMeterTimestamp;

void parseLine(String line);

#endif // DSMR_PARSER_H
