#pragma once

#include "types.h"

void initSensors();

float readTemperature();
float readHumidity();
bool readMotion();
int readLight();
int readGas();

SensorData readAllSensors();
void noteMotionIfDetected(bool motion);
unsigned long lastMotionTimestamp();
