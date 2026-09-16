#include <Arduino.h>

#include "config.h"
#include "pins.h"
#include "types.h"
#include "sensors.h"
#include "devices.h"
#include "automation.h"
#include "cloud.h"
#include "display_oled.h"

static SystemSettings settings;
static DeviceState commanded;
static bool runningManual = false;
static unsigned long lastUploadMs = 0;
static unsigned long lastCommandMs = 0;
static unsigned long lastHistoryMs = 0;
static unsigned long lastDisplayMs = 0;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("========================================");
  Serial.println(" Smart Home Monitoring and Automation");
  Serial.println(" ESP32 student prototype");
  Serial.println("========================================");

  loadDefaultSettings(settings);
  commanded.light = false;
  commanded.fan = false;
  commanded.relay = false;
  commanded.buzzer = false;

  initDevices();
  initSensors();
  initDisplay();
  connectWiFi();
  connectFirebase();
}

void loop() {
  const unsigned long now = millis();

  const SensorData sensors = readAllSensors();

  ensureFirebase();

  if (firebaseReady() && (now - lastCommandMs) >= COMMAND_POLL_INTERVAL_MS) {
    lastCommandMs = now;
    readFirebaseSettings(settings);
    if (isManualMode(settings)) {
      readFirebaseCommands(commanded);
    }
  }

  if (isManualMode(settings)) {
    if (!runningManual) {
      commanded = currentDeviceState();
      runningManual = true;
    }
    manualControl(commanded);
  } else {
    runningManual = false;
    automaticControl(sensors, settings);
  }

  const DeviceState devices = currentDeviceState();

  if (now - lastDisplayMs >= 400) {
    lastDisplayMs = now;
    updateDisplay(sensors, devices, settings, wifiConnected(), firebaseReady());
  }

  if (firebaseReady() && (now - lastUploadMs) >= SENSOR_UPLOAD_INTERVAL_MS) {
    lastUploadMs = now;
    updateFirebase(sensors, devices);
  }

  if (firebaseReady() && (now - lastHistoryMs) >= HISTORY_LOG_INTERVAL_MS) {
    lastHistoryMs = now;
    logHistory(sensors, devices);
  }

  delay(50);
}
