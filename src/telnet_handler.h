#ifndef TELNET_HANDLER_H
#define TELNET_HANDLER_H

void setupTelnet();
void handleTelnet();

void SerialTelnetPrint(const String &message);
void SerialTelnetPrintln(const String &message);
void SerialTelnetPrintF(const char* format, ...);

#endif