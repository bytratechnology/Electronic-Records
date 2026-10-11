#include <Arduino.h>

/*
 * Project 03 — Fire alarm
 * Board: Arduino Uno. Wiring table: ../README.md
 *
 * An IR flame sensor triggers a latching alarm. "Latching" means the buzzer
 * keeps sounding after the flame disappears, until someone presses the
 * button — the same behaviour as a real alarm panel.
 */

#define PIN_FLAME A0       // analog output of the flame module
#define PIN_BUTTON 3       // push button to GND, uses the internal pull-up
#define PIN_BUZZER 8
#define PIN_LED_RED 9
#define PIN_LED_GREEN 10

// On the common KY-026 style module the analog voltage FALLS when the
// sensor sees infrared from a flame. Check your own module on Serial first.
#define FLAME_THRESHOLD 400
#define SAMPLE_INTERVAL_MS 50
#define CONFIRM_SAMPLES 5        // 5 x 50 ms in a row before the alarm starts
#define CLEAR_HOLD_MS 10000UL    // flame-free time needed to re-arm
#define DEBOUNCE_MS 40
#define BLINK_INTERVAL_MS 150
#define REPORT_INTERVAL_MS 500

enum State { MONITORING, ALARM, SILENCED };

static State state = MONITORING;
static int flameReading = 1023;
static uint8_t confirmCount = 0;
static unsigned long lastSampleMs = 0;
static unsigned long lastFlameMs = 0;
static unsigned long lastReportMs = 0;

static bool buttonStable = HIGH;
static bool buttonLastRead = HIGH;
static unsigned long buttonChangedMs = 0;

static bool flameSeen() {
  return flameReading < FLAME_THRESHOLD;
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

static void setOutputs(bool red, bool green, bool buzzer) {
  digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN, green ? HIGH : LOW);
  digitalWrite(PIN_BUZZER, buzzer ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  setOutputs(false, true, false);
  Serial.println("flame,threshold");
}

void loop() {
  const unsigned long now = millis();
  const bool pressed = buttonPressed(now);

  if (now - lastSampleMs >= SAMPLE_INTERVAL_MS) {
    lastSampleMs = now;
    flameReading = analogRead(PIN_FLAME);

    if (flameSeen()) {
      lastFlameMs = now;
      if (confirmCount < CONFIRM_SAMPLES) {
        confirmCount++;
      }
    } else {
      confirmCount = 0;
    }
  }

  switch (state) {
    case MONITORING:
      setOutputs(false, true, false);
      if (confirmCount >= CONFIRM_SAMPLES) {
        state = ALARM;
        Serial.println("# ALARM: flame confirmed");
      }
      break;

    case ALARM: {
      const bool blink = (now / BLINK_INTERVAL_MS) % 2 == 0;
      setOutputs(blink, false, blink);
      if (pressed) {
        state = SILENCED;
        Serial.println("# Silenced by button");
      }
      break;
    }

    case SILENCED:
      // Buzzer off, red LED stays on as a reminder until the area is clear.
      setOutputs(true, false, false);
      if (now - lastFlameMs >= CLEAR_HOLD_MS) {
        state = MONITORING;
        confirmCount = 0;
        Serial.println("# Re-armed");
      }
      break;
  }

  if (now - lastReportMs >= REPORT_INTERVAL_MS) {
    lastReportMs = now;
    Serial.print(flameReading);
    Serial.print(",");
    Serial.println(FLAME_THRESHOLD);
  }
}
