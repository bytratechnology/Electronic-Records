#pragma once

#include "types.h"

void automaticControl(const SensorData& sensors, const SystemSettings& settings);
void manualControl(const DeviceState& commanded);
