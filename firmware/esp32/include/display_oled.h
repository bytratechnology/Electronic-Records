#pragma once

#include "types.h"

void initDisplay();
void updateDisplay(const SensorData& sensors,
                   const DeviceState& devices,
                   const SystemSettings& settings,
                   bool wifiOk,
                   bool firebaseOk);
