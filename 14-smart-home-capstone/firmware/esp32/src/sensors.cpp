#include "sensors.h"
#include "pins.h"

#include <Arduino.h>
#include <DHT.h>

static DHT dht(PIN_DHT, DHT_TYPE);

static float cachedTemperature = NAN;
static float cachedHumidity = NAN;
static bool cachedDhtOk = false;
static unsigned long lastDhtReadMs = 0;
static unsigned long lastMotionMs = 0;

void initSensors() {
  dht.begin();
  pinMode(PIN_PIR, INPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_LDR, ADC_11db);
  analogSetPinAttenuation(PIN_GAS, ADC_11db);

  Serial.println("[sensors] DHT, PIR, LDR, MQ-2 ready");
  Serial.println("[sensors] MQ-2 needs 1-3 minutes to warm up");
}

static bool refreshDht() {
  const unsigned long now = millis();
  if (now - lastDhtReadMs < DHT_MIN_INTERVAL_MS && lastDhtReadMs != 0) {
    return cachedDhtOk;
  }

  const float temperature = dht.readTemperature();
  const float humidity = dht.readHumidity();

  lastDhtReadMs = now;

  if (isnan(temperature) || isnan(humidity)) {
    cachedDhtOk = false;
    Serial.println("[sensors] DHT read failed");
    return false;
  }

  cachedTemperature = temperature;
  cachedHumidity = humidity;
  cachedDhtOk = true;
  return true;
}

float readTemperature() {
  refreshDht();
  return cachedTemperature;
}

float readHumidity() {
  refreshDht();
  return cachedHumidity;
}

bool readMotion() {
  return digitalRead(PIN_PIR) == HIGH;
}

int readLight() {
  return analogRead(PIN_LDR);
}

int readGas() {
  return analogRead(PIN_GAS);
}

void noteMotionIfDetected(bool motion) {
  if (motion) {
    lastMotionMs = millis();
  }
}

unsigned long lastMotionTimestamp() {
  return lastMotionMs;
}

SensorData readAllSensors() {
  SensorData data;
  data.dhtOk = refreshDht();
  data.temperature = cachedTemperature;
  data.humidity = cachedHumidity;
  data.motion = readMotion();
  data.light = readLight();
  data.gas = readGas();
  noteMotionIfDetected(data.motion);
  return data;
}
