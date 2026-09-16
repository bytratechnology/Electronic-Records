#include "cloud.h"
#include "config.h"
#include "paths.h"
#include "pins.h"

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <string.h>

static FirebaseData fbdo;
static FirebaseAuth auth;
static FirebaseConfig fbConfig;
static bool firebaseStarted = false;
static bool defaultsSeeded = false;

void loadDefaultSettings(SystemSettings& settings) {
  strncpy(settings.mode, "auto", sizeof(settings.mode) - 1);
  settings.mode[sizeof(settings.mode) - 1] = '\0';
  settings.temperatureThreshold = 30.0f;
  settings.lightThreshold = 1500;
  settings.gasThreshold = 1800;
  settings.motionHoldMs = MOTION_HOLD_MS_DEFAULT;
}

bool wifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void connectWiFi() {
  if (wifiConnected()) {
    return;
  }

  Serial.printf("[wifi] Connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long start = millis();
  while (!wifiConnected() && (millis() - start) < WIFI_CONNECT_TIMEOUT_MS) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();

  if (wifiConnected()) {
    Serial.printf("[wifi] Connected, IP %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("[wifi] Connect timed out — will retry. Sensors still run.");
  }
}

void ensureWiFi() {
  if (wifiConnected()) {
    return;
  }

  static unsigned long lastAttemptMs = 0;
  const unsigned long now = millis();
  if (lastAttemptMs != 0 && (now - lastAttemptMs) < FIREBASE_RETRY_INTERVAL_MS) {
    return;
  }
  lastAttemptMs = now;

  Serial.println("[wifi] retry (non-blocking)");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectFirebase() {
  if (!wifiConnected() || firebaseStarted) {
    return;
  }

  fbConfig.api_key = FIREBASE_API_KEY;
  fbConfig.database_url = FIREBASE_DATABASE_URL;
  auth.user.email = FIREBASE_USER_EMAIL;
  auth.user.password = FIREBASE_USER_PASSWORD;
  fbConfig.token_status_callback = tokenStatusCallback;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  fbdo.setBSSLBufferSize(4096, 1024);

  Firebase.begin(&fbConfig, &auth);
  firebaseStarted = true;
  Serial.println("[firebase] begin() called, waiting for token");
}

static void seedDefaultsIfNeeded() {
  if (defaultsSeeded || !Firebase.ready()) {
    return;
  }

  if (Firebase.RTDB.getJSON(&fbdo, FB_SETTINGS) && fbdo.jsonObjectPtr() != nullptr) {
    FirebaseJsonData check;
    if (fbdo.jsonObjectPtr()->get(check, "mode") && check.success) {
      defaultsSeeded = true;
      return;
    }
  }

  FirebaseJson json;
  json.set("mode", "auto");
  json.set("temperature_threshold", 30.0);
  json.set("light_threshold", 1500);
  json.set("gas_threshold", 1800);
  json.set("motion_hold_ms", static_cast<int>(MOTION_HOLD_MS_DEFAULT));

  if (Firebase.RTDB.updateNode(&fbdo, FB_SETTINGS, &json)) {
    Serial.println("[firebase] wrote default settings");
    defaultsSeeded = true;
  }
}

bool firebaseReady() {
  return wifiConnected() && firebaseStarted && Firebase.ready();
}

void ensureFirebase() {
  ensureWiFi();
  if (!wifiConnected()) {
    return;
  }
  if (!firebaseStarted) {
    connectFirebase();
  }
  if (firebaseReady()) {
    seedDefaultsIfNeeded();
  }
}

static bool okOrLog(const char* action) {
  if (fbdo.httpCode() < 0) {
    Serial.printf("[firebase] %s failed: %s\n", action, fbdo.errorReason().c_str());
    return false;
  }
  return true;
}

void updateFirebase(const SensorData& sensors, const DeviceState& devices) {
  if (!firebaseReady()) {
    return;
  }

  FirebaseJson json;
  if (sensors.dhtOk) {
    json.set("temperature", sensors.temperature);
    json.set("humidity", sensors.humidity);
  }
  json.set("motion", sensors.motion);
  json.set("light", sensors.light);
  json.set("gas", sensors.gas);

  if (!Firebase.RTDB.updateNode(&fbdo, FB_SENSORS, &json)) {
    okOrLog("update sensors");
    return;
  }

  publishDeviceStates(devices);
  publishOnlineStatus(true);

  if (!Firebase.RTDB.setTimestamp(&fbdo, FB_LAST_UPDATE)) {
    okOrLog("timestamp");
  }
}

void publishDeviceStates(const DeviceState& devices) {
  if (!firebaseReady()) {
    return;
  }

  FirebaseJson json;
  json.set("light", devices.light);
  json.set("fan", devices.fan);
  json.set("relay", devices.relay);
  json.set("buzzer", devices.buzzer);

  if (!Firebase.RTDB.updateNode(&fbdo, FB_DEVICES, &json)) {
    okOrLog("update devices");
  }
}

void publishOnlineStatus(bool online) {
  if (!firebaseReady()) {
    return;
  }
  if (!Firebase.RTDB.setBool(&fbdo, FB_ONLINE, online)) {
    okOrLog("online");
  }
}

bool readFirebaseSettings(SystemSettings& settings) {
  if (!firebaseReady()) {
    return false;
  }

  if (!Firebase.RTDB.getJSON(&fbdo, FB_SETTINGS)) {
    okOrLog("get settings");
    return false;
  }

  FirebaseJson* json = fbdo.jsonObjectPtr();
  if (json == nullptr) {
    return false;
  }

  FirebaseJsonData result;

  if (json->get(result, "mode") && result.success) {
    strncpy(settings.mode, result.to<String>().c_str(), sizeof(settings.mode) - 1);
    settings.mode[sizeof(settings.mode) - 1] = '\0';
  }

  if (json->get(result, "temperature_threshold") && result.success) {
    settings.temperatureThreshold = result.to<float>();
  }

  if (json->get(result, "light_threshold") && result.success) {
    settings.lightThreshold = result.to<int>();
  }

  if (json->get(result, "gas_threshold") && result.success) {
    settings.gasThreshold = result.to<int>();
  }

  if (json->get(result, "motion_hold_ms") && result.success) {
    const int hold = result.to<int>();
    if (hold > 0) {
      settings.motionHoldMs = static_cast<unsigned long>(hold);
    }
  }

  return true;
}

bool readFirebaseCommands(DeviceState& commanded) {
  if (!firebaseReady()) {
    return false;
  }

  if (!Firebase.RTDB.getJSON(&fbdo, FB_DEVICES)) {
    okOrLog("get devices");
    return false;
  }

  FirebaseJson* json = fbdo.jsonObjectPtr();
  if (json == nullptr) {
    return false;
  }

  FirebaseJsonData result;

  if (json->get(result, "light") && result.success) {
    commanded.light = result.to<bool>();
  }
  if (json->get(result, "fan") && result.success) {
    commanded.fan = result.to<bool>();
  }
  if (json->get(result, "relay") && result.success) {
    commanded.relay = result.to<bool>();
  }
  if (json->get(result, "buzzer") && result.success) {
    commanded.buzzer = result.to<bool>();
  }

  return true;
}

void logHistory(const SensorData& sensors, const DeviceState& devices) {
  if (!firebaseReady()) {
    return;
  }

  FirebaseJson json;
  if (sensors.dhtOk) {
    json.set("temperature", sensors.temperature);
    json.set("humidity", sensors.humidity);
  }
  json.set("motion", sensors.motion);
  json.set("light", sensors.light);
  json.set("gas", sensors.gas);
  json.set("light_on", devices.light);
  json.set("fan_on", devices.fan);
  json.set("timestamp/.sv", "timestamp");

  if (!Firebase.RTDB.pushJSON(&fbdo, FB_HISTORY, &json)) {
    okOrLog("history");
  }
}
