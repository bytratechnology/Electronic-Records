#pragma once

/*
 * Copy this file to include/config.h and fill in your own values.
 * include/config.h is gitignored. Never commit real credentials.
 */

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Web app URL from Apps Script > Deploy > Manage deployments.
// It ends in /exec. Treat it like a password: anyone with the URL and the
// secret below can add rows to the sheet.
#define SCRIPT_URL "https://script.google.com/macros/s/YOUR_DEPLOYMENT_ID/exec"

// Must be the same string as SHARED_SECRET in apps-script/Code.gs
#define SCRIPT_SECRET "CHANGE_ME"

// Written into the "Device" column, useful when there is more than one reader.
#define DEVICE_NAME "classroom-door"
