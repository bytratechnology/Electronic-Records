#pragma once

#include "types.h"

void initDevices();

void controlLight(bool on);
void controlFan(bool on);
void controlRelay(bool on);
void controlBuzzer(bool on);

void applyDeviceState(const DeviceState& state);
DeviceState currentDeviceState();
