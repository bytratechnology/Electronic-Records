#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

#include "config.h"
#include "page.h"

/*
 * Project 06 — Online temperature and humidity station
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * The ESP32 is its own web server. The browser loads one page from "/" and
 * then polls two JSON endpoints, so the page and the data are separate.
 */

#define PIN_DHT 4
#define DHT_TYPE DHT22

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define DHT_INTERVAL_MS 2500
#define HISTORY_INTERVAL_MS 60000
#define HISTORY_SIZE 60  // one point per minute = the last hour

static DHT dht(PIN_DHT, DHT_TYPE);
static WebServer server(80);

static float temperature = NAN;
static float humidity = NAN;
static bool sensorOk = false;

// Ring buffer: historyHead is the slot the next sample will be written to.
static float historyTemperature[HISTORY_SIZE];
static float historyHumidity[HISTORY_SIZE];
static int historyHead = 0;
static int historyCount = 0;

static unsigned long lastDhtMs = 0;
static unsigned long lastHistoryMs = 0;

static void connectWiFi() {
  Serial.printf("[wifi] Connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_CONNECT_TIMEOUT_MS) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[wifi] Connected. Open http://%s/\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("[wifi] Connect timed out — will retry. Sensor still runs.");
  }
}

static void ensureWiFi() {
  static unsigned long lastAttemptMs = 0;
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }
  const unsigned long now = millis();
  if (now - lastAttemptMs < WIFI_RETRY_INTERVAL_MS) {
    return;
  }
  lastAttemptMs = now;
  Serial.println("[wifi] retry");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// JSON has no NaN, so a missing reading is sent as null.
static String jsonNumber(float value, int decimals) {
  return isnan(value) ? String("null") : String(value, decimals);
}

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handleReadings() {
  String json = "{";
  json += "\"temperature\":" + jsonNumber(temperature, 1);
  json += ",\"humidity\":" + jsonNumber(humidity, 1);
  json += ",\"ok\":" + String(sensorOk ? "true" : "false");
  json += ",\"uptime_s\":" + String(millis() / 1000);
  json += ",\"rssi\":" + String(WiFi.RSSI());
  json += "}";
  server.send(200, "application/json", json);
}

static void handleHistory() {
  String temperatures = "[";
  String humidities = "[";
  // Oldest sample first so the chart reads left to right.
  const int start = (historyHead - historyCount + HISTORY_SIZE) % HISTORY_SIZE;
  for (int i = 0; i < historyCount; i++) {
    const int index = (start + i) % HISTORY_SIZE;
    if (i > 0) {
      temperatures += ",";
      humidities += ",";
    }
    temperatures += jsonNumber(historyTemperature[index], 1);
    humidities += jsonNumber(historyHumidity[index], 1);
  }
  temperatures += "]";
  humidities += "]";

  String json = "{\"interval_s\":" + String(HISTORY_INTERVAL_MS / 1000);
  json += ",\"temperature\":" + temperatures;
  json += ",\"humidity\":" + humidities + "}";
  server.send(200, "application/json", json);
}

static void readSensor() {
  const float t = dht.readTemperature();
  const float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    sensorOk = false;
    Serial.println("[dht] read failed");
    return;
  }
  temperature = t;
  humidity = h;
  sensorOk = true;
}

static void storeHistory() {
  historyTemperature[historyHead] = sensorOk ? temperature : NAN;
  historyHumidity[historyHead] = sensorOk ? humidity : NAN;
  historyHead = (historyHead + 1) % HISTORY_SIZE;
  if (historyCount < HISTORY_SIZE) {
    historyCount++;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 06 — Online weather station");

  dht.begin();
  connectWiFi();

  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("[mdns] http://%s.local/\n", MDNS_NAME);
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/readings", HTTP_GET, handleReadings);
  server.on("/api/history", HTTP_GET, handleHistory);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
}

void loop() {
  const unsigned long now = millis();

  ensureWiFi();
  server.handleClient();

  if (now - lastDhtMs >= DHT_INTERVAL_MS) {
    lastDhtMs = now;
    readSensor();
  }

  if (now - lastHistoryMs >= HISTORY_INTERVAL_MS || historyCount == 0) {
    lastHistoryMs = now;
    storeHistory();
  }
}
