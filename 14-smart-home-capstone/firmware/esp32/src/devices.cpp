#include "devices.h"
#include "pins.h"

#include <Arduino.h>

static DeviceState state = {false, false, false, false};

void initDevices() {
  pinMode(PIN_LIGHT, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  controlLight(false);
  controlFan(false);
  controlRelay(false);
  controlBuzzer(false);

  Serial.println("[devices] light, fan, relay, buzzer ready");
}

void controlLight(bool on) {
  digitalWrite(PIN_LIGHT, on ? HIGH : LOW);
  state.light = on;
}

void controlFan(bool on) {
  digitalWrite(PIN_FAN, on ? HIGH : LOW);
  state.fan = on;
}

void controlRelay(bool on) {
  digitalWrite(PIN_RELAY, on ? RELAY_ON_LEVEL : RELAY_OFF_LEVEL);
  state.relay = on;
}

void controlBuzzer(bool on) {
  digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
  state.buzzer = on;
}

void applyDeviceState(const DeviceState& next) {
  controlLight(next.light);
  controlFan(next.fan);
  controlRelay(next.relay);
  controlBuzzer(next.buzzer);
}

DeviceState currentDeviceState() {
  return state;
}
