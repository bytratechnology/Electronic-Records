#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include "config.h"
#include "page.h"

/*
 * Project 10 — IoT automatic plant watering
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * Water takes minutes to spread through soil, so the controller waters in
 * short bursts: pump for a few seconds, wait ("soak"), measure again. Two
 * limits protect the plant and the desk if the sensor fails: a hard cap on
 * one pump run and a cap on automatic runs per 24 hours.
 */

#define PIN_SOIL 34   // ADC1, input only. Capacitive sensor AOUT (sensor on 3.3 V)
#define PIN_PUMP 26   // relay or MOSFET module input

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define SAMPLE_INTERVAL_MS 500
#define MAX_PUMP_SECONDS 30
#define MAX_AUTO_RUNS_PER_DAY 6
#define DAY_MS 86400000UL

#if PUMP_ACTIVE_LOW
#define PUMP_ON_LEVEL LOW
#define PUMP_OFF_LEVEL HIGH
#else
#define PUMP_ON_LEVEL HIGH
#define PUMP_OFF_LEVEL LOW
#endif

struct Settings {
  bool autoEnabled;
  int startPercent;  // water when moisture falls below this
  int pumpSeconds;
  int soakMinutes;
  int dryRaw;        // ADC reading with the probe in air
  int wetRaw;        // ADC reading with the probe in a glass of water
};

enum Phase { IDLE, PUMPING, SOAKING };

static WebServer server(80);
static Preferences prefs;
static Settings settings;

static Phase phase = IDLE;
static float rawSmoothed = 0;
static int moisturePercent = 0;
static int autoRuns = 0;
static unsigned long phaseStartMs = 0;
static unsigned long dayStartMs = 0;
static unsigned long lastSampleMs = 0;

static void loadSettings() {
  prefs.begin("watering", false);
  settings.autoEnabled = prefs.getBool("auto", true);
  settings.startPercent = prefs.getInt("start", 35);
  settings.pumpSeconds = prefs.getInt("pump", 5);
  settings.soakMinutes = prefs.getInt("soak", 10);
  settings.dryRaw = prefs.getInt("dry", 3000);
  settings.wetRaw = prefs.getInt("wet", 1400);
}

static void saveSettings() {
  prefs.putBool("auto", settings.autoEnabled);
  prefs.putInt("start", settings.startPercent);
  prefs.putInt("pump", settings.pumpSeconds);
  prefs.putInt("soak", settings.soakMinutes);
  prefs.putInt("dry", settings.dryRaw);
  prefs.putInt("wet", settings.wetRaw);
}

static void setPump(bool on) {
  digitalWrite(PIN_PUMP, on ? PUMP_ON_LEVEL : PUMP_OFF_LEVEL);
}

static void startPumping(unsigned long now) {
  phase = PUMPING;
  phaseStartMs = now;
  setPump(true);
}

// A capacitive probe reads HIGH when dry and LOW when wet, so the map is inverted.
static int toPercent(int raw) {
  if (settings.dryRaw == settings.wetRaw) {
    return 0;
  }
  const long percent = map(raw, settings.dryRaw, settings.wetRaw, 0, 100);
  return constrain(percent, 0, 100);
}

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
    Serial.println("[wifi] Connect timed out — will retry. Automatic watering still runs.");
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

static const char* phaseName() {
  switch (phase) {
    case PUMPING: return "watering";
    case SOAKING: return "soaking";
    default: return "idle";
  }
}

static void sendState() {
  String json = "{\"moisture\":" + String(moisturePercent);
  json += ",\"raw\":" + String(static_cast<int>(rawSmoothed));
  json += ",\"pump\":" + String(phase == PUMPING ? "true" : "false");
  json += ",\"phase\":\"" + String(phaseName()) + "\"";
  json += ",\"runs_today\":" + String(autoRuns);
  json += ",\"max_runs\":" + String(MAX_AUTO_RUNS_PER_DAY);
  json += ",\"auto\":" + String(settings.autoEnabled ? "true" : "false");
  json += ",\"start_percent\":" + String(settings.startPercent);
  json += ",\"pump_seconds\":" + String(settings.pumpSeconds);
  json += ",\"soak_minutes\":" + String(settings.soakMinutes);
  json += ",\"dry_raw\":" + String(settings.dryRaw);
  json += ",\"wet_raw\":" + String(settings.wetRaw) + "}";
  server.send(200, "application/json", json);
}

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

// Manual run from the page. Ignored while the pump is already running.
static void handleWater() {
  if (phase != PUMPING) {
    Serial.println("[pump] manual run");
    startPumping(millis());
  }
  sendState();
}

static void handleSettings() {
  if (server.hasArg("auto")) {
    settings.autoEnabled = server.arg("auto") == "1";
  }
  if (server.hasArg("start_percent")) {
    settings.startPercent = constrain(server.arg("start_percent").toInt(), 5, 90);
  }
  if (server.hasArg("pump_seconds")) {
    settings.pumpSeconds = constrain(server.arg("pump_seconds").toInt(), 1, MAX_PUMP_SECONDS);
  }
  if (server.hasArg("soak_minutes")) {
    settings.soakMinutes = constrain(server.arg("soak_minutes").toInt(), 1, 240);
  }
  if (server.hasArg("dry_raw")) {
    settings.dryRaw = constrain(server.arg("dry_raw").toInt(), 0, 4095);
  }
  if (server.hasArg("wet_raw")) {
    settings.wetRaw = constrain(server.arg("wet_raw").toInt(), 0, 4095);
  }
  saveSettings();
  moisturePercent = toPercent(static_cast<int>(rawSmoothed));
  sendState();
}

static void runController(unsigned long now) {
  if (now - dayStartMs >= DAY_MS) {
    dayStartMs = now;
    autoRuns = 0;
  }

  switch (phase) {
    case IDLE:
      if (settings.autoEnabled && moisturePercent < settings.startPercent) {
        if (autoRuns < MAX_AUTO_RUNS_PER_DAY) {
          autoRuns++;
          Serial.printf("[pump] auto run %d, moisture %d%%\n", autoRuns, moisturePercent);
          startPumping(now);
        }
      }
      break;

    case PUMPING: {
      const unsigned long runMs = static_cast<unsigned long>(
          min(settings.pumpSeconds, MAX_PUMP_SECONDS)) * 1000UL;
      if (now - phaseStartMs >= runMs) {
        setPump(false);
        phase = SOAKING;
        phaseStartMs = now;
      }
      break;
    }

    case SOAKING:
      if (now - phaseStartMs >= static_cast<unsigned long>(settings.soakMinutes) * 60000UL) {
        phase = IDLE;
      }
      break;
  }
}

void setup() {
  // Pump off before anything else, so a reset never leaves it running.
  pinMode(PIN_PUMP, OUTPUT);
  setPump(false);

  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 10 — IoT plant watering");

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_SOIL, ADC_11db);
  rawSmoothed = analogRead(PIN_SOIL);

  loadSettings();
  moisturePercent = toPercent(static_cast<int>(rawSmoothed));

  connectWiFi();
  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("[mdns] http://%s.local/\n", MDNS_NAME);
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, sendState);
  server.on("/api/water", HTTP_POST, handleWater);
  server.on("/api/settings", HTTP_POST, handleSettings);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;
    rawSmoothed = 0.8f * rawSmoothed + 0.2f * analogRead(PIN_SOIL);
    moisturePercent = toPercent(static_cast<int>(rawSmoothed));
  }

  runController(now);

  ensureWiFi();
  server.handleClient();
}
