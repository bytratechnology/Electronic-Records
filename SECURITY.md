# Security

This is a student STEM prototype, not a production home-security product.
It still follows a few rules so Wi-Fi passwords and Firebase keys never
end up on GitHub.

## What must never be committed

Do not put any of the following in git, README files, screenshots of code,
or commit messages:

- Wi-Fi SSID/password (if the network is private)
- Firebase API keys used with a real project that has open rules
- Firebase database secrets
- Service account JSON files
- Email/password of the Firebase user
- Personal phone numbers, home address, or school ID numbers

The files that hold real values are:

| File | Purpose |
| ---- | ------- |
| `firmware/esp32/include/config.h` | ESP32 Wi-Fi and Firebase credentials |
| `dashboard/firebase-config.js` | Dashboard Firebase config and login |

Both are listed in `.gitignore`. Only the example files are tracked:

- `firmware/esp32/config.example.h`
- `dashboard/firebase-config.example.js`

## How to set up credentials locally

1. Copy `firmware/esp32/config.example.h` to `firmware/esp32/include/config.h`.
2. Copy `dashboard/firebase-config.example.js` to `dashboard/firebase-config.js`.
3. Fill in values from your own Wi-Fi and Firebase project.
4. Run `git status` and confirm those two files do **not** appear as new files.

## Firebase rules

Do not leave Realtime Database rules as public read/write after a classroom demo.

Recommended classroom rule (requires signed-in user):

```json
{
  "rules": {
    "smart_home": {
      ".read": "auth != null",
      ".write": "auth != null"
    }
  }
}
```

Open rules (`.read: true`, `.write: true`) make the database reachable by
anyone who knows the URL. Use them only for a few minutes of local testing,
then switch back to authenticated rules.

This project uses **email/password authentication**, not a database secret.
The ESP32 and the dashboard sign in as the same Firebase user.

## Hardware safety

This model uses low-voltage DC loads only (LED, small DC fan, 5 V relay
switching a DC load). Do not connect mains 220 V / 110 V to the relay
module in this prototype.

The MQ-2 analog output can exceed 3.3 V. Use the voltage divider described
in `hardware/wiring.md` before connecting it to an ESP32 ADC pin.

## If credentials leak

1. Change the Wi-Fi password if it was committed.
2. In Firebase: disable the leaked user, rotate the web API key if needed,
   and tighten database rules.
3. Remove the secret from git history before making the repository public.
4. Assume the leaked value is compromised; do not keep using it.
