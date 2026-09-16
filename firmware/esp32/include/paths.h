#pragma once

/*
 * Firebase Realtime Database paths.
 * The dashboard must use the same tree: window.FIREBASE_ROOT = "smart_home"
 */
#define FB_ROOT "/smart_home"

#define FB_SENSORS FB_ROOT "/sensors"
#define FB_DEVICES FB_ROOT "/devices"
#define FB_SETTINGS FB_ROOT "/settings"
#define FB_SYSTEM FB_ROOT "/system"
#define FB_HISTORY FB_ROOT "/history"

#define FB_TEMPERATURE FB_SENSORS "/temperature"
#define FB_HUMIDITY FB_SENSORS "/humidity"
#define FB_MOTION FB_SENSORS "/motion"
#define FB_LIGHT FB_SENSORS "/light"
#define FB_GAS FB_SENSORS "/gas"

#define FB_DEV_LIGHT FB_DEVICES "/light"
#define FB_DEV_FAN FB_DEVICES "/fan"
#define FB_DEV_RELAY FB_DEVICES "/relay"
#define FB_DEV_BUZZER FB_DEVICES "/buzzer"

#define FB_MODE FB_SETTINGS "/mode"
#define FB_TEMP_TH FB_SETTINGS "/temperature_threshold"
#define FB_LIGHT_TH FB_SETTINGS "/light_threshold"
#define FB_GAS_TH FB_SETTINGS "/gas_threshold"
#define FB_MOTION_HOLD FB_SETTINGS "/motion_hold_ms"

#define FB_ONLINE FB_SYSTEM "/online"
#define FB_LAST_UPDATE FB_SYSTEM "/last_update"
