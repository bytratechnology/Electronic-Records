# Dashboard

A static web page that reads and writes Firebase Realtime Database.
No build step. HTML, CSS, and JavaScript only, so it is easy to demo.

## Files

| File | Role |
| ---- | ---- |
| `index.html` | Layout |
| `style.css` | Styling |
| `script.js` | Firebase listeners and controls |
| `firebase-config.example.js` | Template for keys |
| `firebase-config.js` | Your real config (gitignored) |

The page talks to the same tree as the ESP32: `smart_home/`.

## One-time config

```bash
cp dashboard/firebase-config.example.js dashboard/firebase-config.js
```

Fill in the web app config from Firebase Console → Project settings → Your apps,
and the same email/password user that the ESP32 uses.

Do not commit `firebase-config.js`.

## Run locally

Firebase and ES-style script loading expect http, not `file://`.

```bash
cd dashboard
python3 -m http.server 8080
```

Open http://localhost:8080

The dashboard also needs:

1. Email/password authentication enabled.
2. Realtime Database created.
3. Rules that allow the signed-in user to read and write `smart_home`
   (see `firebase/database.rules.json`).

## What you can do in the UI

- Watch temperature, humidity, light, motion, and gas in near real time.
- Switch **AUTO** / **MANUAL**.
- In Manual mode, toggle light, fan, relay, and buzzer.
- Change thresholds stored in `smart_home/settings`.
- Download recent history as CSV.

In Auto mode the toggles are disabled on purpose. They still show the state
the ESP32 published.

## Offline / stale ESP32

If `system/last_update` is older than about 20 seconds, the pill shows
offline even if `system/online` is still true. That covers the case where
the board lost power without writing `online: false`.
