#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "supabase_client.h"
#include "telnet_handler.h"

/*
 * The device sends the raw telegram; the server parses it.
 *
 * This used to POST ~19 pre-parsed fields straight at a Supabase table with the anon key. Monkey
 * Suite's `p1-ingest` edge function expects the opposite split: `{ meter_id, device_token, raw }`,
 * where `raw` is the untouched telegram, and its own `parseTelegram()` does the OBIS mapping before
 * inserting a `p1_readings` row.
 *
 * That split is deliberate, and it is why nothing is lost by dropping the field-by-field JSON:
 *
 *  - `p1_readings.raw` is `not null`, so every telegram is stored verbatim. Anything the server
 *    parser does not extract *yet* — power failures, voltage sags and swells, the gas meter's own
 *    id — is still sitting in the stored text and becomes queryable by extending the parser and
 *    adding columns. That is a redeploy, not a re-flash of a device inside a meter cupboard.
 *  - A parser bug is fixed server-side for every household at once.
 *
 * Authentication is the `device_token` in the body, checked against the `meters` table inside the
 * function. There is no `apikey` header because the function is deployed `--no-verify-jwt` — the
 * ESP32 has no Supabase user session to present.
 */
/** Describes a credential without disclosing it: length, the ends, and whether it has stray edges. */
static String describe(const char* value, size_t expected) {
  const String s(value);
  String out = "len=" + String(s.length());
  if (expected && s.length() != expected) out += " (EXPECTED " + String(expected) + ")";
  if (s.length() && (isspace((unsigned char)s[0]) || isspace((unsigned char)s[s.length() - 1]))) {
    out += " HAS LEADING/TRAILING WHITESPACE";
  }
  // Ends only. Enough to spot a mismatch against the database by eye, useless to anyone reading the log.
  if (s.length() > 8) out += "  " + s.substring(0, 4) + "…" + s.substring(s.length() - 4);
  return out
;
}

void logIngestConfig() {
  /*
   * The three values a 401 can be blamed on, and the two things that make a correct-looking value
   * wrong: an unexpected length (a paste that lost its tail) and whitespace at either end (a paste
   * that carried a newline). `meter_id` is `'p1_' || 10 hex` = 13 chars; `device_token` is
   * `encode(gen_random_bytes(24),'hex')` = 48. Anything else did not come from register_meter().
   */
  SerialTelnetPrintln("Ingest config:");
  SerialTelnetPrint("  url      ");
  SerialTelnetPrintln(ingest_url);
  SerialTelnetPrint("  meter_id ");
  SerialTelnetPrint(meter_id);
  SerialTelnetPrint("  ");
  SerialTelnetPrintln(describe(meter_id, 13));
  SerialTelnetPrint("  token    ");
  SerialTelnetPrintln(describe(device_token, 48));
}

void sendDataToSupabase(const String& raw) {
  if (WiFi.status() != WL_CONNECTED) {
    SerialTelnetPrintln("WiFi not connected");
    return;
  }

  /*
   * `setInsecure()` — the endpoint is HTTPS and the ESP32 ships no CA store.
   *
   * Without an explicit client, `HTTPClient::begin(url)` on an https URL fails the handshake with no
   * useful message, which is a genuinely miserable thing to debug in a meter cupboard. Skipping chain
   * validation is the pragmatic default here: the token is the actual credential, the payload is meter
   * readings, and Supabase rotates its certificate. To harden this, pin the ISRG Root X1 CA with
   * `client.setCACert(...)` instead — but then a CA rotation bricks ingest until someone re-flashes.
   */
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  if (!http.begin(client, ingest_url)) {
    SerialTelnetPrintln("Ingest: http.begin() failed — check ingest_url");
    return;
  }
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["meter_id"] = meter_id;
  doc["device_token"] = device_token;
  doc["raw"] = raw;  // ArduinoJson escapes the CRLFs and quotes for us.

  String requestBody;
  serializeJson(doc, requestBody);

  const int status = http.POST(requestBody);
  if (status > 0) {
    const String response = http.getString();
    SerialTelnetPrint("Ingest ");
    SerialTelnetPrint(String(status));
    SerialTelnetPrint(": ");
    SerialTelnetPrintln(response);
    // 401 is the one worth calling out by name — it is almost always the wrong `meter_id`: the
    // telegram carries the meter's *equipment* id, which is not what register_meter() minted.
    if (status == 401) {
      SerialTelnetPrintln("  -> unauthorized: no meters row matches BOTH meter_id and device_token");
      logIngestConfig();
    }
  } else {
    SerialTelnetPrint("Ingest failed, HTTPClient error: ");
    SerialTelnetPrintln(http.errorToString(status));
  }
  http.end();
}
