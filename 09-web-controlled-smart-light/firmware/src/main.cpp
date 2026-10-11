#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "config.h"
#include "page.h"

/*
 * Project 09 — Smart light controlled from a web page
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * One LED "lamp" with three ways to change it: the web page, a physical
 * push button, and an off-timer. All three go through setLight(), so the
 * page always shows the real state no matter who changed it.
 */

#define PIN_LAMP 25    // LED through 220 ohm, dimmed with PWM
#define PIN_BUTTON 14  // push button to GND, internal pull-up

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define DEBOUNCE_MS 40
#define MIN_BRIGHTNESS 5
#define MAX_BRIGHTNESS 255
#define MAX_TIMER_S 7200

static WebServer server(80);

static bool lightOn = false;
static int brightness = MAX_BRIGHTNESS;
static unsigned long offAtMs = 0;  // 0 = no timer running
static bool timerActive = false;

static bool buttonStable = HIGH;
static bool buttonLastRead = HIGH;
static unsigned long buttonChangedMs = 0;

static void applyOutput() {
  analogWrite(PIN_LAMP, lightOn ? brightness : 0);
}

static void setLight(bool on) {
  lightOn = on;
  if (!on) {
    timerActive = false;
  }
  applyOutput();
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
    Serial.println("[wifi] Connect timed out — will retry. The button still works.");
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

static unsigned long timerSecondsLeft() {
  if (!timerActive) {
    return 0;
  }
  const long remaining = static_cast<long>(offAtMs - millis());
  return remaining > 0 ? (remaining + 999) / 1000 : 0;
}

static void sendState() {
  String json = "{\"on\":" + String(lightOn ? "true" : "false");
  json += ",\"brightness\":" + String(brightness);
  json += ",\"timer_s\":" + String(timerSecondsLeft()) + "}";
  server.send(200, "application/json", json);
}

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

// POST /api/light with any of: on=0|1, brightness=5..255, timer=<seconds>
static void handleLight() {
  if (server.hasArg("brightness")) {
    brightness = constrain(server.arg("brightness").toInt(), MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    applyOutput();
  }
  if (server.hasArg("on")) {
    setLight(server.arg("on") == "1");
  }
  if (server.hasArg("timer")) {
    const long seconds = constrain(server.arg("timer").toInt(), 0, MAX_TIMER_S);
    timerActive = seconds > 0 && lightOn;
    offAtMs = millis() + static_cast<unsigned long>(seconds) * 1000UL;
  }
  sendState();
}

// Returns true once per press (falling edge after debounce).
static bool buttonPressed(unsigned long now) {
  const bool reading = digitalRead(PIN_BUTTON);
  if (reading != buttonLastRead) {
    buttonLastRead = reading;
    buttonChangedMs = now;
  }
  if (now - buttonChangedMs > DEBOUNCE_MS && reading != buttonStable) {
    buttonStable = reading;
    return buttonStable == LOW;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 09 — Web-controlled smart light");

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LAMP, OUTPUT);
  applyOutput();

  connectWiFi();
  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("[mdns] http://%s.local/\n", MDNS_NAME);
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, sendState);
  server.on("/api/light", HTTP_POST, handleLight);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
}

void loop() {
  const unsigned long now = millis();

  if (buttonPressed(now)) {
    setLight(!lightOn);
    Serial.println(lightOn ? "[button] on" : "[button] off");
  }

  // Signed difference keeps the comparison correct when millis() wraps.
  if (timerActive && static_cast<long>(now - offAtMs) >= 0) {
    setLight(false);
    Serial.println("[timer] off");
  }

  ensureWiFi();
  server.handleClient();
}
