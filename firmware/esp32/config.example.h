#pragma once

/*
 * Copy this file to include/config.h and fill in your own values.
 * include/config.h is gitignored. Never commit real credentials.
 */

// --- Wi-Fi ---
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// --- Firebase Realtime Database ---
// Project settings > General > Your apps > Web app
#define FIREBASE_API_KEY "YOUR_API_KEY"

// Project settings > Service accounts > Database secrets is NOT used.
// Use the Realtime Database URL from the RTDB console, for example:
// https://your-project-default-rtdb.asia-southeast1.firebasedatabase.app
#define FIREBASE_DATABASE_URL "https://YOUR_PROJECT-default-rtdb.REGION.firebasedatabase.app"

// Authentication > Authentication > Email/Password user
#define FIREBASE_USER_EMAIL "YOUR_FIREBASE_EMAIL"
#define FIREBASE_USER_PASSWORD "YOUR_FIREBASE_PASSWORD"

// --- Hardware options ---
// DHT22 is more accurate; DHT11 also works. Change DHT_TYPE in pins.h
// if you use DHT11.

// Set to 0 if you do not have an SSD1306 OLED module.
#define ENABLE_OLED 1
