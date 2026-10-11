#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#include "config.h"

/*
 * Project 08 — Real-time fire alert
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * Two independent signs of fire (infrared from a flame, or high temperature)
 * raise the alarm. The state is pushed over MQTT, so a subscribed browser
 * hears about it within a moment instead of polling for it.
 *
 * Topics under MQTT_TOPIC_BASE:
 *   status     "online" / "offline"   retained, "offline" is the last will
 *   alarm      "FIRE" / "CLEAR"       retained, published on every change
 *   telemetry  JSON                   every 5 s
 */

#define PIN_FLAME 34   // ADC1, input only. Flame module AO (module on 3.3 V)
#define PIN_DHT 4
#define DHT_TYPE DHT22
#define PIN_BUZZER 25
#define PIN_LED 26

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define MQTT_RETRY_INTERVAL_MS 5000
#define SAMPLE_INTERVAL_MS 50
#define CONFIRM_SAMPLES 5          // 250 ms of continuous flame signal
#define FLAME_THRESHOLD 1500       // analog value FALLS when a flame is seen
#define TEMPERATURE_ALARM_C 55.0f
#define CLEAR_HOLD_MS 10000UL      // both signs must be gone this long
#define DHT_INTERVAL_MS 2500
#define TELEMETRY_INTERVAL_MS 5000

static WiFiClient net;
static PubSubClient mqtt(net);
static DHT dht(PIN_DHT, DHT_TYPE);

static int flameReading = 4095;
static uint8_t confirmCount = 0;
static float temperature = NAN;
static bool alarmActive = false;
static bool alarmDirty = true;  // true = the broker does not have the current state yet

static unsigned long lastSampleMs = 0;
static unsigned long lastDangerMs = 0;
static unsigned long lastDhtMs = 0;
static unsigned long lastTelemetryMs = 0;

static String topic(const char* leaf) {
  return String(MQTT_TOPIC_BASE) + "/" + leaf;
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
  Serial.println(WiFi.status() == WL_CONNECTED
                     ? "[wifi] Connected"
                     : "[wifi] Connect timed out — will retry. Local alarm still works.");
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

static void ensureMqtt() {
  static unsigned long lastAttemptMs = 0;
  if (mqtt.connected() || WiFi.status() != WL_CONNECTED) {
    return;
  }
  const unsigned long now = millis();
  if (lastAttemptMs != 0 && now - lastAttemptMs < MQTT_RETRY_INTERVAL_MS) {
    return;
  }
  lastAttemptMs = now;

  const String clientId = "fire-alarm-" + String(static_cast<uint32_t>(ESP.getEfuseMac()), HEX);
  const String statusTopic = topic("status");
  const char* user = strlen(MQTT_USER) > 0 ? MQTT_USER : nullptr;
  const char* password = strlen(MQTT_PASSWORD) > 0 ? MQTT_PASSWORD : nullptr;

  // Last will: if the ESP32 loses power, the broker publishes "offline" for it.
  if (mqtt.connect(clientId.c_str(), user, password, statusTopic.c_str(), 1, true, "offline")) {
    Serial.println("[mqtt] connected");
    mqtt.publish(statusTopic.c_str(), "online", true);
    alarmDirty = true;  // re-send the alarm state after every reconnect
  } else {
    Serial.printf("[mqtt] connect failed, state %d\n", mqtt.state());
  }
}

static void publishAlarmIfNeeded() {
  if (!alarmDirty || !mqtt.connected()) {
    return;
  }
  if (mqtt.publish(topic("alarm").c_str(), alarmActive ? "FIRE" : "CLEAR", true)) {
    alarmDirty = false;
  }
}

static void publishTelemetry() {
  if (!mqtt.connected()) {
    return;
  }
  String json = "{\"flame\":" + String(flameReading);
  json += ",\"temperature\":" + (isnan(temperature) ? String("null") : String(temperature, 1));
  json += ",\"alarm\":" + String(alarmActive ? "true" : "false");
  json += ",\"uptime_s\":" + String(millis() / 1000) + "}";
  mqtt.publish(topic("telemetry").c_str(), json.c_str());
}

static void setAlarm(bool active) {
  if (alarmActive == active) {
    return;
  }
  alarmActive = active;
  alarmDirty = true;
  Serial.println(active ? "[alarm] FIRE" : "[alarm] CLEAR");
}

static void detect(unsigned long now) {
  if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;
    flameReading = analogRead(PIN_FLAME);
    if (flameReading < FLAME_THRESHOLD) {
      if (confirmCount < CONFIRM_SAMPLES) {
        confirmCount++;
      }
    } else {
      confirmCount = 0;
    }
  }

  if (now - lastDhtMs >= DHT_INTERVAL_MS) {
    lastDhtMs = now;
    const float t = dht.readTemperature();
    if (!isnan(t)) {
      temperature = t;
    }
  }

  const bool flameConfirmed = confirmCount >= CONFIRM_SAMPLES;
  const bool tooHot = !isnan(temperature) && temperature >= TEMPERATURE_ALARM_C;

  if (flameConfirmed || tooHot) {
    lastDangerMs = now;
    setAlarm(true);
  } else if (alarmActive && now - lastDangerMs >= CLEAR_HOLD_MS) {
    setAlarm(false);
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 08 — Real-time fire alert");

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_FLAME, ADC_11db);
  dht.begin();

  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setSocketTimeout(3);  // keep a dead broker from freezing the buzzer for long

  connectWiFi();
}

void loop() {
  const unsigned long now = millis();

  // 1. Detection and local alarm first.
  detect(now);
  const bool blink = (now / 150) % 2 == 0;
  digitalWrite(PIN_LED, alarmActive ? HIGH : LOW);
  digitalWrite(PIN_BUZZER, alarmActive && blink ? HIGH : LOW);

  // 2. Network.
  ensureWiFi();
  ensureMqtt();
  mqtt.loop();
  publishAlarmIfNeeded();

  if (now - lastTelemetryMs >= TELEMETRY_INTERVAL_MS) {
    lastTelemetryMs = now;
    publishTelemetry();
  }
}
