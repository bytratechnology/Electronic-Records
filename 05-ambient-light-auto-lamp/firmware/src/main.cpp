#include <Arduino.h>

/*
 * Project 05 — Ambient-light automatic lamp
 * Board: Arduino Uno. Wiring table: ../README.md
 *
 * An LDR measures room brightness and switches a lamp at dusk and dawn.
 * A single threshold makes the lamp flicker when the light level sits right
 * on it, so this sketch uses three protections: smoothing, hysteresis, and
 * a minimum time between switches.
 */

#define PIN_LDR A0   // LDR to 5 V, 10 k to GND: brighter room = higher value
#define PIN_POT A1   // potentiometer sets the switching level
#define PIN_LAMP 9

#define SAMPLE_INTERVAL_MS 50
#define HYSTERESIS 40          // ADC counts either side of the set point
#define MIN_DWELL_MS 3000UL    // lamp must hold a state at least this long
#define REPORT_INTERVAL_MS 250

static float light = 0;
static bool lampOn = false;
static unsigned long lastSampleMs = 0;
static unsigned long lastSwitchMs = 0;
static unsigned long lastReportMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LAMP, OUTPUT);
  digitalWrite(PIN_LAMP, LOW);
  light = analogRead(PIN_LDR);
  Serial.println("light,turn_on_below,turn_off_above,lamp");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  // Exponential moving average hides a hand shadow or a camera flash.
  light = 0.9f * light + 0.1f * analogRead(PIN_LDR);

  const int setPoint = analogRead(PIN_POT);
  const int turnOnBelow = setPoint - HYSTERESIS;
  const int turnOffAbove = setPoint + HYSTERESIS;

  const bool dwellPassed = now - lastSwitchMs >= MIN_DWELL_MS;
  if (dwellPassed) {
    if (!lampOn && light < turnOnBelow) {
      lampOn = true;
      lastSwitchMs = now;
    } else if (lampOn && light > turnOffAbove) {
      lampOn = false;
      lastSwitchMs = now;
    }
  }
  digitalWrite(PIN_LAMP, lampOn ? HIGH : LOW);

  if (now - lastReportMs >= REPORT_INTERVAL_MS) {
    lastReportMs = now;
    Serial.print(static_cast<int>(light));
    Serial.print(",");
    Serial.print(turnOnBelow);
    Serial.print(",");
    Serial.print(turnOffAbove);
    Serial.print(",");
    Serial.println(lampOn ? 1000 : 0);  // scaled so it is visible on the plotter
  }
}
