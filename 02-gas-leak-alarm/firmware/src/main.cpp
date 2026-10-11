#include <Arduino.h>

/*
 * Project 02 — Gas leak alarm
 * Board: Arduino Uno. Wiring table: ../README.md
 *
 * The MQ-2 does not give ppm. This sketch measures the clean-air level after
 * warm-up and raises the alarm when the reading climbs a fixed margin above
 * that baseline. Two margins (on / off) stop the buzzer chattering.
 */

#define PIN_GAS A0
#define PIN_BUZZER 8
#define PIN_LED_RED 9
#define PIN_LED_GREEN 10

#define WARMUP_MS 60000UL       // MQ-2 heater needs time before readings settle
#define SAMPLE_INTERVAL_MS 100
#define BASELINE_SAMPLES 50
#define ALARM_MARGIN 150        // ADC counts above baseline that start the alarm
#define CLEAR_MARGIN 100        // must fall below this margin to stop it
#define BEEP_INTERVAL_MS 200
#define REPORT_INTERVAL_MS 1000

enum State { WARMING_UP, CALIBRATING, MONITORING, ALARM };

static State state = WARMING_UP;
static float smoothed = 0;
static long baselineSum = 0;
static int baselineCount = 0;
static int baseline = 0;
static unsigned long lastSampleMs = 0;
static unsigned long lastBeepMs = 0;
static unsigned long lastReportMs = 0;
static bool beepOn = false;

static void setOutputs(bool red, bool green, bool buzzer) {
  digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN, green ? HIGH : LOW);
  digitalWrite(PIN_BUZZER, buzzer ? HIGH : LOW);
}

static void report(unsigned long now) {
  if (now - lastReportMs < REPORT_INTERVAL_MS) {
    return;
  }
  lastReportMs = now;

  // gas, alarm-on level, alarm-off level — three lines on the Serial Plotter
  Serial.print(static_cast<int>(smoothed));
  Serial.print(",");
  Serial.print(baseline + ALARM_MARGIN);
  Serial.print(",");
  Serial.println(baseline + CLEAR_MARGIN);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  setOutputs(false, false, false);

  smoothed = analogRead(PIN_GAS);
  Serial.println("# Warming up MQ-2 for 60 s");
}

void loop() {
  const unsigned long now = millis();

  if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;
    // Exponential moving average: 90 % old value, 10 % new sample.
    smoothed = 0.9f * smoothed + 0.1f * analogRead(PIN_GAS);

    if (state == CALIBRATING) {
      baselineSum += static_cast<long>(smoothed);
      baselineCount++;
      if (baselineCount >= BASELINE_SAMPLES) {
        baseline = baselineSum / baselineCount;
        state = MONITORING;
        Serial.print("# Baseline: ");
        Serial.println(baseline);
        Serial.println("gas,alarm_on,alarm_off");
      }
    }
  }

  switch (state) {
    case WARMING_UP:
      // Slow green blink so it is obvious the alarm is not armed yet.
      setOutputs(false, (now / 500) % 2 == 0, false);
      if (now >= WARMUP_MS) {
        state = CALIBRATING;
      }
      break;

    case CALIBRATING:
      setOutputs(false, (now / 150) % 2 == 0, false);
      break;

    case MONITORING:
      setOutputs(false, true, false);
      if (smoothed > baseline + ALARM_MARGIN) {
        state = ALARM;
        Serial.println("# ALARM: gas above threshold");
      }
      report(now);
      break;

    case ALARM:
      if (now - lastBeepMs >= BEEP_INTERVAL_MS) {
        lastBeepMs = now;
        beepOn = !beepOn;
      }
      setOutputs(true, false, beepOn);
      if (smoothed < baseline + CLEAR_MARGIN) {
        state = MONITORING;
        beepOn = false;
        Serial.println("# Cleared");
      }
      report(now);
      break;
  }
}
