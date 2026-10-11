#pragma once

/*
 * Copy this file to include/config.h and fill in your own values.
 * include/config.h is gitignored. Never commit real credentials.
 */

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Create a bot with @BotFather in Telegram to get the token.
// Anyone holding this token can send messages as the bot — keep it private.
#define TELEGRAM_BOT_TOKEN "YOUR_BOT_TOKEN"

// Send any message to your bot, then open
// https://api.telegram.org/bot<TOKEN>/getUpdates and copy "chat":{"id":...}
#define TELEGRAM_CHAT_ID "YOUR_CHAT_ID"

// Appears at the start of every message.
#define DEVICE_NAME "Kitchen gas sensor"
