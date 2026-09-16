#include "automation.h"
#include "devices.h"
#include "sensors.h"

#include <Arduino.h>

void automaticControl(const SensorData& sensors, const SystemSettings& settings) {
  const bool occupied =
      sensors.motion ||
      (lastMotionTimestamp() != 0 &&
       (millis() - lastMotionTimestamp()) < settings.motionHoldMs);

  const bool fanOn =
      sensors.dhtOk && sensors.temperature > settings.temperatureThreshold;

  const bool dark = sensors.light < settings.lightThreshold;
  const bool lightOn = dark && occupied;

  const bool gasAlert = sensors.gas > settings.gasThreshold;

  controlFan(fanOn);
  controlLight(lightOn);
  controlBuzzer(gasAlert);

  /*
   * Relay stays as-is in automatic mode. It is a general-purpose output
   * for a low-voltage extra load and is only driven from Manual mode.
   */
}

void manualControl(const DeviceState& commanded) {
  applyDeviceState(commanded);
}
