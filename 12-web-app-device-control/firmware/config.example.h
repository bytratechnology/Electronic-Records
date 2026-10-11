#pragma once

/*
 * Copy this file to include/config.h and fill in your own values.
 * include/config.h is gitignored. Never commit real credentials.
 */

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Firebase console > Project settings > General > Your apps > Web app
#define FIREBASE_API_KEY "YOUR_API_KEY"

// Realtime Database URL, for example:
// https://your-project-default-rtdb.asia-southeast1.firebasedatabase.app
#define FIREBASE_DATABASE_URL "https://YOUR_PROJECT-default-rtdb.REGION.firebasedatabase.app"

// A dedicated Email/Password user for the board (Authentication > Users).
#define FIREBASE_USER_EMAIL "YOUR_DEVICE_EMAIL"
#define FIREBASE_USER_PASSWORD "YOUR_DEVICE_PASSWORD"

// Most 4-channel relay boards switch ON when the input is LOW.
#define RELAY_ACTIVE_LOW 1
