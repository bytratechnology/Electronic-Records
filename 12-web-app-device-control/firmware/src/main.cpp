#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

#include "config.h"

/*
 * Project 12 — Device control from a web app
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * The web app never talks to the ESP32 directly. It writes what the user
 * WANTS under device_control/desired. The ESP32 listens to that node with a
 * stream, switches the relays, then writes what actually HAPPENED under
 * device_control/reported. The app shows "pending" until the two agree.
 */

#define CHANNEL_COUNT 4
static const uint8_t RELAY_PINS[CHANNEL_COUNT] = {16, 17, 18, 19};
static const char* const CHANNEL_KEYS[CHANNEL_COUNT] = {"ch1", "ch2", "ch3", "ch4"};

#define PATH_DESIRED "/device_control/desired"
#define PATH_REPORTED "/device_control/reported"
#define PATH_LAST_SEEN "/device_control/status/last_seen"

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define HEARTBEAT_INTERVAL_MS 10000

#if RELAY_ACTIVE_LOW
#define RELAY_ON_LEVEL LOW
#define RELAY_OFF_LEVEL HIGH
#else
#define RELAY_ON_LEVEL HIGH
#define RELAY_OFF_LEVEL LOW
#endif

static FirebaseData stream;  // long-lived connection for incoming commands
static FirebaseData fbdo;    // short requests (reported state, heartbeat)
static FirebaseAuth auth;
static FirebaseConfig fbConfig;

// The stream callback runs in a separate task, so it only records the
// request. loop() does the actual pin and network work.
static volatile bool desired[CHANNEL_COUNT] = {false, false, false, false};
static volatile bool desiredChanged = false;

static bool applied[CHANNEL_COUNT] = {false, false, false, false};
static bool firebaseStarted = false;
static bool streamStarted = false;
static bool reportPending = true;
static unsigned long lastHeartbeatMs = 0;

static void writeRelay(int channel, bool on) {
  digitalWrite(RELAY_PINS[channel], on ? RELAY_ON_LEVEL : RELAY_OFF_LEVEL);
  applied[channel] = on;
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
                     : "[wifi] Connect timed out — will retry. Relays keep their state.");
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

static void onStream(FirebaseStream data) {
  const String type = data.dataType();
  const String path = data.dataPath();

  if (type == "json") {
    // First event after (re)connecting: the whole "desired" node.
    FirebaseJson* json = data.to<FirebaseJson*>();
    FirebaseJsonData value;
    for (int i = 0; i < CHANNEL_COUNT; i++) {
      if (json->get(value, CHANNEL_KEYS[i]) && value.success) {
        desired[i] = value.to<bool>();
      }
    }
  } else if (type == "boolean") {
    // Later events: one changed child, for example "/ch2".
    for (int i = 0; i < CHANNEL_COUNT; i++) {
      if (path == String("/") + CHANNEL_KEYS[i]) {
        desired[i] = data.to<bool>();
      }
    }
  }
  desiredChanged = true;
}

static void onStreamTimeout(bool timedOut) {
  if (timedOut) {
    Serial.println("[firebase] stream timed out, the library will resume it");
  }
}

static void startFirebase() {
  fbConfig.api_key = FIREBASE_API_KEY;
  fbConfig.database_url = FIREBASE_DATABASE_URL;
  auth.user.email = FIREBASE_USER_EMAIL;
  auth.user.password = FIREBASE_USER_PASSWORD;
  fbConfig.token_status_callback = tokenStatusCallback;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&fbConfig, &auth);
  firebaseStarted = true;
  Serial.println("[firebase] begin() called, waiting for token");
}

static void ensureFirebase() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  if (!firebaseStarted) {
    startFirebase();
  }
  if (!streamStarted && Firebase.ready()) {
    if (Firebase.RTDB.beginStream(&stream, PATH_DESIRED)) {
      Firebase.RTDB.setStreamCallback(&stream, onStream, onStreamTimeout);
      streamStarted = true;
      Serial.println("[firebase] listening for commands");
    } else {
      Serial.printf("[firebase] stream failed: %s\n", stream.errorReason().c_str());
    }
  }
}

static void applyDesired() {
  if (!desiredChanged) {
    return;
  }
  desiredChanged = false;

  for (int i = 0; i < CHANNEL_COUNT; i++) {
    const bool want = desired[i];
    if (want != applied[i]) {
      writeRelay(i, want);
      Serial.printf("[relay] %s -> %s\n", CHANNEL_KEYS[i], want ? "ON" : "OFF");
    }
  }
  reportPending = true;
}

static void reportState() {
  if (!reportPending || !Firebase.ready()) {
    return;
  }
  FirebaseJson json;
  for (int i = 0; i < CHANNEL_COUNT; i++) {
    json.set(CHANNEL_KEYS[i], applied[i]);
  }
  if (Firebase.RTDB.updateNode(&fbdo, PATH_REPORTED, &json)) {
    reportPending = false;
  } else {
    Serial.printf("[firebase] report failed: %s\n", fbdo.errorReason().c_str());
  }
}

static void heartbeat(unsigned long now) {
  if (!Firebase.ready() || now - lastHeartbeatMs < HEARTBEAT_INTERVAL_MS) {
    return;
  }
  lastHeartbeatMs = now;
  // Server-side timestamp: the web app compares it with its own clock.
  if (!Firebase.RTDB.setTimestamp(&fbdo, PATH_LAST_SEEN)) {
    Serial.printf("[firebase] heartbeat failed: %s\n", fbdo.errorReason().c_str());
  }
}

void setup() {
  // Relays off before anything else, so a reset never leaves a load on.
  for (int i = 0; i < CHANNEL_COUNT; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    writeRelay(i, false);
  }

  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 12 — Web app device control");

  connectWiFi();
}

void loop() {
  const unsigned long now = millis();

  ensureWiFi();
  ensureFirebase();

  applyDesired();
  reportState();
  heartbeat(now);

  delay(20);
}
