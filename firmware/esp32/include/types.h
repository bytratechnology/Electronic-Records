#pragma once

struct SensorData {
  float temperature;
  float humidity;
  bool motion;
  int light;
  int gas;
  bool dhtOk;
};

struct DeviceState {
  bool light;
  bool fan;
  bool relay;
  bool buzzer;
};

struct SystemSettings {
  char mode[12];
  float temperatureThreshold;
  int lightThreshold;
  int gasThreshold;
  unsigned long motionHoldMs;
};

bool isManualMode(const SystemSettings& settings);
