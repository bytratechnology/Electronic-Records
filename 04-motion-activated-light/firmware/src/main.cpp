#include <Arduino.h>

/*
 * Project 04 — Motion-activated light
 * Board: Arduino Uno. Wiring table: ../README.md
 *
 * A PIR sensor switches an LED on. The LED stays on for a hold time after
 * the last movement and then fades out instead of snapping off. Everything
 * is timed with millis(), so there is no delay() in the main loop.
 */

#define PIN_PIR 2
#define PIN_LAMP 9  // PWM pin: LED through 220 ohm (or a MOSFET gate)

#define PIR_WARMUP_MS 30000UL  // HC-SR501 gives false triggers after power-up
#define HOLD_MS 15000UL        // stay on this long after the last motion
#define FADE_MS 2000UL
#define FULL_BRIGHTNESS 255

enum State { WARMING_UP, OFF, ON, FADING };

static State state = WARMING_UP;
static unsigned long lastMotionMs = 0;
static unsigned long fadeStartMs = 0;
static unsigned int triggerCount = 0;

static void switchOn(unsigned long now) {
  if (state == OFF) {
    triggerCount++;
    Serial.print("Motion #");
    Serial.println(triggerCount);
  }
  lastMotionMs = now;
  state = ON;
  analogWrite(PIN_LAMP, FULL_BRIGHTNESS);
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_PIR, INPUT);
  pinMode(PIN_LAMP, OUTPUT);
  analogWrite(PIN_LAMP, 0);
  Serial.println("PIR warming up (30 s)");
}

void loop() {
  const unsigned long now = millis();
  const bool motion = digitalRead(PIN_PIR) == HIGH;

  switch (state) {
    case WARMING_UP:
      if (now >= PIR_WARMUP_MS) {
        state = OFF;
        Serial.println("Ready");
      }
      break;

    case OFF:
      if (motion) {
        switchOn(now);
      }
      break;

    case ON:
      if (motion) {
        lastMotionMs = now;
      } else if (now - lastMotionMs >= HOLD_MS) {
        state = FADING;
        fadeStartMs = now;
      }
      break;

    case FADING: {
      if (motion) {
        switchOn(now);
        break;
      }
      const unsigned long elapsed = now - fadeStartMs;
      if (elapsed >= FADE_MS) {
        analogWrite(PIN_LAMP, 0);
        state = OFF;
        Serial.println("Light off");
      } else {
        analogWrite(PIN_LAMP, map(elapsed, 0, FADE_MS, FULL_BRIGHTNESS, 0));
      }
      break;
    }
  }
}
