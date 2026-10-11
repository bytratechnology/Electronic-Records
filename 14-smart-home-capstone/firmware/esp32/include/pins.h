#pragma once

#include <Arduino.h>
#include <DHT.h>

/*
 * GPIO map for a 30-pin ESP32-WROOM development board.
 * Avoids strapping pins (0, 2, 12, 15) for outputs.
 * ADC pins 34 and 35 are input-only, which is correct for LDR and MQ-2.
 *
 * Keep this file, hardware/wiring.md, and the dashboard docs in sync.
 */

// --- Sensors ---
#define PIN_DHT 4
#define DHT_TYPE DHT22

#define PIN_PIR 27
#define PIN_LDR 34  // ADC1_CH6
#define PIN_GAS 35  // ADC1_CH7 (use voltage divider — see wiring.md)

// --- Actuators ---
#define PIN_LIGHT 25   // LED through 220 ohm resistor
#define PIN_FAN 26     // NPN transistor driving 5 V DC fan
#define PIN_RELAY 32   // Relay module IN pin
#define PIN_BUZZER 33  // Active buzzer

// --- OLED (I2C, optional) ---
#define PIN_OLED_SDA 21
#define PIN_OLED_SCL 22
#define OLED_ADDRESS 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

/*
 * Most cheap 5 V relay modules are active-LOW (IN = GND turns the coil on).
 * Set to 0 if your module is active-HIGH.
 */
#define RELAY_ACTIVE_LOW 1

#if RELAY_ACTIVE_LOW
#define RELAY_ON_LEVEL LOW
#define RELAY_OFF_LEVEL HIGH
#else
#define RELAY_ON_LEVEL HIGH
#define RELAY_OFF_LEVEL LOW
#endif

// --- Timing ---
#define WIFI_CONNECT_TIMEOUT_MS 20000
#define SENSOR_UPLOAD_INTERVAL_MS 2000
#define COMMAND_POLL_INTERVAL_MS 1000
#define HISTORY_LOG_INTERVAL_MS 60000
#define DHT_MIN_INTERVAL_MS 2000
#define MOTION_HOLD_MS_DEFAULT 10000
#define FIREBASE_RETRY_INTERVAL_MS 5000
