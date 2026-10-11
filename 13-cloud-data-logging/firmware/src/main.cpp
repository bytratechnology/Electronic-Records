#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <time.h>

#include "config.h"

/*
 * Project 13 — Cloud data logging
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * "Store and forward": every reading is time-stamped on the ESP32 and put
 * in a queue. A separate step uploads the oldest reading first. If the
 * Wi-Fi drops, readings wait in the queue and are uploaded later with their
 * original times, so the cloud log has no hole in it.
 */

#define PIN_DHT 4
#define DHT_TYPE DHT22
#define PIN_LDR 34  // ADC1, input only. LDR to 3.3 V, 10 k to GND

#define PATH_READINGS "/data_logger/readings"

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define UPLOAD_RETRY_INTERVAL_MS 5000
#define QUEUE_SIZE 120                 // two hours of readings at one per minute
#define CLOCK_VALID_AFTER 1700000000L  // any time before late 2023 means "not synced"

struct Reading {
  time_t timestamp;  // UTC, seconds
  float temperature;
  float humidity;
  int light;
};

static DHT dht(PIN_DHT, DHT_TYPE);
static FirebaseData fbdo;
static FirebaseAuth auth;
static FirebaseConfig fbConfig;
static bool firebaseStarted = false;

// Ring buffer: queueHead is the oldest reading.
static Reading queue[QUEUE_SIZE];
static int queueHead = 0;
static int queueCount = 0;
static unsigned long droppedCount = 0;

static unsigned long lastSampleMs = 0;
static unsigned long lastUploadAttemptMs = 0;
static bool firstSampleTaken = false;

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
                     : "[wifi] Connect timed out — will retry.");
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

static void ensureFirebase() {
  if (firebaseStarted || WiFi.status() != WL_CONNECTED) {
    return;
  }
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

static bool clockSynced() {
  return time(nullptr) > CLOCK_VALID_AFTER;
}

static void enqueue(const Reading& reading) {
  if (queueCount >= QUEUE_SIZE) {
    // Full: give up the oldest reading to keep the newest.
    queueHead = (queueHead + 1) % QUEUE_SIZE;
    queueCount--;
    droppedCount++;
    Serial.printf("[queue] full, dropped oldest (%lu dropped in total)\n", droppedCount);
  }
  queue[(queueHead + queueCount) % QUEUE_SIZE] = reading;
  queueCount++;
}

static void takeSample() {
  // A reading without a real time cannot be placed on the chart, so the
  // logger waits for the first NTP sync. After that the ESP32 keeps time
  // by itself, even with the Wi-Fi down.
  if (!clockSynced()) {
    Serial.println("[sample] skipped, clock not synced yet");
    return;
  }

  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("[sample] DHT read failed, skipped");
    return;
  }

  Reading reading;
  reading.timestamp = time(nullptr);
  reading.temperature = temperature;
  reading.humidity = humidity;
  reading.light = analogRead(PIN_LDR);
  enqueue(reading);

  Serial.printf("[sample] %.1f C, %.0f %%, light %d (%d queued)\n",
                temperature, humidity, reading.light, queueCount);
}

static void uploadOldest(unsigned long now) {
  if (queueCount == 0 || !firebaseStarted || !Firebase.ready()) {
    return;
  }
  if (lastUploadAttemptMs != 0 && now - lastUploadAttemptMs < UPLOAD_RETRY_INTERVAL_MS) {
    return;
  }

  const Reading& reading = queue[queueHead];
  FirebaseJson json;
  json.set("ts", static_cast<int>(reading.timestamp));
  json.set("temperature", reading.temperature);
  json.set("humidity", reading.humidity);
  json.set("light", reading.light);

  if (Firebase.RTDB.pushJSON(&fbdo, PATH_READINGS, &json)) {
    // Remove from the queue only after the server confirmed the write.
    queueHead = (queueHead + 1) % QUEUE_SIZE;
    queueCount--;
    lastUploadAttemptMs = 0;
  } else {
    lastUploadAttemptMs = now;
    Serial.printf("[firebase] upload failed: %s\n", fbdo.errorReason().c_str());
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 13 — Cloud data logging");

  dht.begin();
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_LDR, ADC_11db);

  connectWiFi();
  // UTC from NTP. The viewer page converts to the reader's local time.
  configTime(0, 0, "pool.ntp.org", "time.google.com");
}

void loop() {
  const unsigned long now = millis();

  ensureWiFi();
  ensureFirebase();

  const unsigned long intervalMs = static_cast<unsigned long>(SAMPLE_INTERVAL_S) * 1000UL;
  if (!firstSampleTaken ? clockSynced() : now - lastSampleMs >= intervalMs) {
    firstSampleTaken = true;
    lastSampleMs = now;
    takeSample();
  }

  uploadOldest(now);

  delay(20);
}
