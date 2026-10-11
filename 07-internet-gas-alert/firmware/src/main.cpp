#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "config.h"

/*
 * Project 07 — Gas alert over the Internet
 * Board: ESP32 DevKit. Wiring table: ../README.md
 *
 * Same detection idea as project 02 (baseline + hysteresis), plus a Telegram
 * message when the alarm starts and when it clears. The local buzzer never
 * waits for the network: detection runs first, messaging second.
 */

#define PIN_GAS 34     // ADC1, input only. MQ-2 A0 through a 10k/10k divider
#define PIN_BUZZER 25
#define PIN_LED 26

#define WIFI_CONNECT_TIMEOUT_MS 20000
#define WIFI_RETRY_INTERVAL_MS 10000
#define WARMUP_MS 60000UL
#define SAMPLE_INTERVAL_MS 100
#define BASELINE_SAMPLES 50
#define ALARM_MARGIN 400          // 12-bit ADC counts above the clean-air level
#define CLEAR_MARGIN 250
#define SEND_RETRY_INTERVAL_MS 10000
#define REMINDER_INTERVAL_MS 300000UL  // repeat the warning every 5 minutes
#define REPORT_INTERVAL_MS 2000

enum State { WARMING_UP, CALIBRATING, MONITORING, ALARM };

static State state = WARMING_UP;
static float smoothed = 0;
static long baselineSum = 0;
static int baselineCount = 0;
static int baseline = 0;

// Only the newest message is kept. If the alarm starts and clears while the
// Wi-Fi is down, the phone receives the latest state, not a stale backlog.
static String pendingMessage;

static unsigned long lastSampleMs = 0;
static unsigned long lastSendAttemptMs = 0;
static unsigned long lastReminderMs = 0;
static unsigned long lastReportMs = 0;

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

static String urlEncode(const String& text) {
  String out;
  char buffer[4];
  for (size_t i = 0; i < text.length(); i++) {
    const unsigned char c = text[i];
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      out += static_cast<char>(c);
    } else {
      snprintf(buffer, sizeof(buffer), "%%%02X", c);
      out += buffer;
    }
  }
  return out;
}

static bool sendTelegram(const String& text) {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  WiFiClientSecure client;
  // Encrypts the connection but does not verify Telegram's certificate.
  // Listed under Limitations in the README.
  client.setInsecure();

  HTTPClient https;
  https.setTimeout(8000);
  if (!https.begin(client, "https://api.telegram.org/bot" TELEGRAM_BOT_TOKEN "/sendMessage")) {
    return false;
  }
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");

  const String body = "chat_id=" + urlEncode(TELEGRAM_CHAT_ID) + "&text=" + urlEncode(text);
  const int code = https.POST(body);
  https.end();

  if (code != 200) {
    Serial.printf("[telegram] send failed, HTTP %d\n", code);
    return false;
  }
  Serial.println("[telegram] sent");
  return true;
}

static void queueMessage(const String& text) {
  pendingMessage = String(DEVICE_NAME) + ": " + text;
  lastSendAttemptMs = 0;  // try on the next loop
}

static void flushMessage(unsigned long now) {
  if (pendingMessage.isEmpty()) {
    return;
  }
  if (lastSendAttemptMs != 0 && now - lastSendAttemptMs < SEND_RETRY_INTERVAL_MS) {
    return;
  }
  lastSendAttemptMs = now;
  if (sendTelegram(pendingMessage)) {
    pendingMessage = "";
  }
}

static void sampleGas() {
  smoothed = 0.9f * smoothed + 0.1f * analogRead(PIN_GAS);

  if (state != CALIBRATING) {
    return;
  }
  baselineSum += static_cast<long>(smoothed);
  baselineCount++;
  if (baselineCount >= BASELINE_SAMPLES) {
    baseline = baselineSum / baselineCount;
    state = MONITORING;
    Serial.printf("[gas] baseline %d, alarm above %d\n", baseline, baseline + ALARM_MARGIN);
    queueMessage("online and monitoring.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\nProject 07 — Internet gas alert");

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_LED, LOW);

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_GAS, ADC_11db);
  smoothed = analogRead(PIN_GAS);

  connectWiFi();
  Serial.println("[gas] warming up MQ-2 for 60 s");
}

void loop() {
  const unsigned long now = millis();

  // 1. Detection and local alarm — never blocked by the network.
  if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;
    sampleGas();
  }

  switch (state) {
    case WARMING_UP:
      digitalWrite(PIN_LED, (now / 500) % 2 == 0 ? HIGH : LOW);
      if (now >= WARMUP_MS) {
        state = CALIBRATING;
      }
      break;

    case CALIBRATING:
      digitalWrite(PIN_LED, (now / 150) % 2 == 0 ? HIGH : LOW);
      break;

    case MONITORING:
      digitalWrite(PIN_LED, LOW);
      digitalWrite(PIN_BUZZER, LOW);
      if (smoothed > baseline + ALARM_MARGIN) {
        state = ALARM;
        lastReminderMs = now;
        queueMessage("GAS ALARM. Reading " + String(static_cast<int>(smoothed)) +
                     ", clean-air level " + String(baseline) + ".");
      }
      break;

    case ALARM: {
      const bool beep = (now / 200) % 2 == 0;
      digitalWrite(PIN_LED, HIGH);
      digitalWrite(PIN_BUZZER, beep ? HIGH : LOW);

      if (smoothed < baseline + CLEAR_MARGIN) {
        state = MONITORING;
        queueMessage("gas level back to normal.");
      } else if (now - lastReminderMs >= REMINDER_INTERVAL_MS) {
        lastReminderMs = now;
        queueMessage("gas alarm still active. Reading " + String(static_cast<int>(smoothed)) + ".");
      }
      break;
    }
  }

  // 2. Network work.
  ensureWiFi();
  flushMessage(now);

  if (now - lastReportMs >= REPORT_INTERVAL_MS) {
    lastReportMs = now;
    Serial.printf("[gas] %d (baseline %d)\n", static_cast<int>(smoothed), baseline);
  }
}
