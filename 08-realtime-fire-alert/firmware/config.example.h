#pragma once

/*
 * Copy this file to include/config.h and fill in your own values.
 * include/config.h is gitignored. Never commit real credentials.
 */

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// MQTT broker. A public test broker (for example broker.hivemq.com) is fine
// for a classroom demo, but anyone can read and write its topics.
#define MQTT_HOST "YOUR_BROKER_HOST"
#define MQTT_PORT 1883

// Leave both empty ("") for a broker without accounts.
#define MQTT_USER ""
#define MQTT_PASSWORD ""

// All topics start with this. Make the last part unique so you do not
// collide with other people on a shared broker.
#define MQTT_TOPIC_BASE "students/fire-alarm/CHANGE_ME"
