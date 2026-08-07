#ifndef SUPABASE_CLIENT_H
#define SUPABASE_CLIENT_H

#include <Arduino.h>

/**
 * POST one raw DSMR telegram to the Monkey Suite `p1-ingest` edge function.
 *
 * `raw` is the telegram exactly as it came off the P1 port. The server parses it — see the note in
 * supabase_client.cpp for why the device no longer sends parsed fields.
 */
void sendDataToSupabase(const String& raw);

/**
 * Print what this device is actually sending, with the token masked.
 *
 * Called at boot and again on every 401, because a telnet client connects long after boot and would
 * otherwise never see it. Reports lengths and flags stray whitespace — a 401 with credentials that
 * "look right" is nearly always a paste that carried a newline or lost its tail.
 */
void logIngestConfig();

#endif // SUPABASE_CLIENT_H
