#pragma once

#include "types.h"

/*
 * Cloud sync (Firebase RTDB).
 * Named cloud.* instead of firebase.* because the Mobizt library already
 * ships Firebase.h, which clashes on case-insensitive filesystems (macOS).
 */

void connectWiFi();
void ensureWiFi();
bool wifiConnected();

void connectFirebase();
bool firebaseReady();
void ensureFirebase();

void updateFirebase(const SensorData& sensors, const DeviceState& devices);
bool readFirebaseCommands(DeviceState& commanded);
bool readFirebaseSettings(SystemSettings& settings);
void publishDeviceStates(const DeviceState& devices);
void publishOnlineStatus(bool online);
void logHistory(const SensorData& sensors, const DeviceState& devices);

void loadDefaultSettings(SystemSettings& settings);
