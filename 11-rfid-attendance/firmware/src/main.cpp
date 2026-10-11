#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

#include "config.h"

/*
 * Project 11 — RFID attendance system
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * A card tap is read by the RC522, time-stamped on the ESP32, and sent to a
 * Google Apps Script web app that appends a row to a spreadsheet. Scans are
 * queued in RAM first, so a tap is never lost just because the Wi-Fi was
 * down at that second.
 */

// RC522 on the default VSPI bus: SCK 18, MISO 19, MOSI 23. The module is 3.3 V only.
#define PIN_RFID_SS 5
#define PIN_RFID_RST 22
#define PIN_BUZZER 25
#define PIN_LED_GREEN 26
#define PIN_LED_RED 27

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define SAME_CARD_BLOCK_MS 5000    // ignore a card held against the reader
#define UPLOAD_RETRY_INTERVAL_MS 5000
#define QUEUE_SIZE 20
#define UID_TEXT_SIZE 21           // up to 10 UID bytes as hex + terminator
#define CLOCK_VALID_AFTER 1700000000L  // any time before late 2023 means "not synced"

struct Scan {
  char uid[UID_TEXT_SIZE];
  time_t scannedAt;  // 0 when the clock was not synced yet
};

static MFRC522 rfid(PIN_RFID_SS, PIN_RFID_RST);

// Ring buffer of scans waiting for upload.
static Scan queue[QUEUE_SIZE];
static int queueHead = 0;
static int queueCount = 0;

static char lastUid[UID_TEXT_SIZE] = "";
static unsigned long lastScanMs = 0;
static unsigned long lastUploadAttemptMs = 0;

// Non-blocking feedback: outputs switch off again at feedbackOffMs.
static unsigned long feedbackOffMs = 0;

static void feedback(bool green, bool red, unsigned long durationMs) {
  digitalWrite(PIN_LED_GREEN, green ? HIGH : LOW);
  digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
  digitalWrite(PIN_BUZZER, HIGH);
  feedbackOffMs = millis() + durationMs;
}

static void updateFeedback(unsigned long now) {
  if (feedbackOffMs != 0 && static_cast<long>(now - feedbackOffMs) >= 0) {
    feedbackOffMs = 0;
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_BUZZER, LOW);
  }
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
                     : "[wifi] Connect timed out — will retry. Scans are queued.");
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

static void uidToText(const MFRC522::Uid& uid, char* out) {
  out[0] = '\0';
  for (byte i = 0; i < uid.size && i < 10; i++) {
    snprintf(out + i * 2, 3, "%02X", uid.uidByte[i]);
  }
}

static bool enqueue(const char* uid) {
  if (queueCount >= QUEUE_SIZE) {
    return false;
  }
  const time_t now = time(nullptr);
  Scan& slot = queue[(queueHead + queueCount) % QUEUE_SIZE];
  strncpy(slot.uid, uid, UID_TEXT_SIZE - 1);
  slot.uid[UID_TEXT_SIZE - 1] = '\0';
  slot.scannedAt = now > CLOCK_VALID_AFTER ? now : 0;
  queueCount++;
  return true;
}

static void readCard(unsigned long now) {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  char uid[UID_TEXT_SIZE];
  uidToText(rfid.uid, uid);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  if (strcmp(uid, lastUid) == 0 && now - lastScanMs < SAME_CARD_BLOCK_MS) {
    return;
  }
  strncpy(lastUid, uid, UID_TEXT_SIZE);
  lastScanMs = now;

  if (enqueue(uid)) {
    Serial.printf("[rfid] %s queued (%d waiting)\n", uid, queueCount);
    feedback(true, false, 120);
  } else {
    Serial.println("[rfid] queue full, scan rejected");
    feedback(false, true, 800);
  }
}

// Returns true when the server answered, so the scan can leave the queue.
static bool upload(const Scan& scan) {
  WiFiClientSecure client;
  // Encrypts the connection but does not verify Google's certificate.
  // Listed under Limitations in the README.
  client.setInsecure();

  HTTPClient https;
  https.setTimeout(10000);
  // Apps Script answers a POST with a 302 redirect to the real response.
  https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (!https.begin(client, SCRIPT_URL)) {
    return false;
  }
  https.addHeader("Content-Type", "application/json");

  String body = "{\"secret\":\"" SCRIPT_SECRET "\",\"device\":\"" DEVICE_NAME "\"";
  body += ",\"uid\":\"" + String(scan.uid) + "\"";
  body += ",\"scanned_at\":" + String(static_cast<long>(scan.scannedAt)) + "}";

  const int code = https.POST(body);
  const String response = code > 0 ? https.getString() : String();
  https.end();

  if (code != 200) {
    Serial.printf("[upload] HTTP %d\n", code);
    return false;
  }

  if (response.indexOf("\"status\":\"ok\"") >= 0) {
    Serial.printf("[upload] %s recorded\n", scan.uid);
    feedback(true, false, 250);
  } else if (response.indexOf("\"status\":\"unknown\"") >= 0) {
    Serial.printf("[upload] %s is not in the Students sheet\n", scan.uid);
    feedback(false, true, 600);
  } else {
    // "denied" or an HTML error page: the request reached Google but was
    // refused. Retrying the same scan would loop forever, so drop it.
    Serial.printf("[upload] rejected: %s\n", response.substring(0, 80).c_str());
    feedback(false, true, 600);
  }
  return true;
}

static void flushQueue(unsigned long now) {
  if (queueCount == 0 || WiFi.status() != WL_CONNECTED) {
    return;
  }
  if (lastUploadAttemptMs != 0 && now - lastUploadAttemptMs < UPLOAD_RETRY_INTERVAL_MS) {
    return;
  }

  if (upload(queue[queueHead])) {
    queueHead = (queueHead + 1) % QUEUE_SIZE;
    queueCount--;
    lastUploadAttemptMs = 0;  // send the next one straight away
  } else {
    lastUploadAttemptMs = now;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 11 — RFID attendance");

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_DumpVersionToSerial();  // prints 0x00 or 0xFF when the wiring is wrong

  connectWiFi();
  // UTC from NTP. The spreadsheet converts to the sheet's own time zone.
  configTime(0, 0, "pool.ntp.org", "time.google.com");
}

void loop() {
  const unsigned long now = millis();

  readCard(now);
  updateFeedback(now);

  ensureWiFi();
  flushQueue(now);
}
