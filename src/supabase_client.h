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

#endif // SUPABASE_CLIENT_H
