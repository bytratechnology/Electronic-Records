# Firebase setup

Classroom steps to connect the ESP32 and the dashboard to one Realtime Database.

These are setup notes, not production cloud architecture.

## 1. Create a project

1. Open [Firebase Console](https://console.firebase.google.com/).
2. Create a project (Google Analytics can be disabled).
3. Do not put the project ID into public screenshots if you used open rules.

## 2. Enable Authentication

1. Build → Authentication → Sign-in method.
2. Enable **Email/Password**.
3. Add a user, for example `smarthome@your-school.example`.
4. Use this same user in:
   - `firmware/esp32/include/config.h`
   - `dashboard/firebase-config.js`

## 3. Create Realtime Database

1. Build → Realtime Database → Create database.
2. Choose a region close to you (the URL region must match `FIREBASE_DATABASE_URL`).
3. Start in locked mode.
4. Replace rules with `firebase/database.rules.json`.

## 4. Register a web app

1. Project settings → Your apps → Web.
2. Copy `apiKey`, `authDomain`, `databaseURL`, `projectId` into
   `dashboard/firebase-config.js`.
3. Copy `apiKey` and `databaseURL` into `firmware/esp32/include/config.h`.

This project does **not** use the legacy database secret.

## 5. Optional seed

In Realtime Database → Data, import `firebase/seed.json`, or let the ESP32
write default settings on first successful connection.

## Tree

See `docs/firebase-database.md` for the schema used by firmware and dashboard.

## Quotas

The firmware uploads sensors every 2 seconds and a history node every 60 seconds.
That is acceptable on the Spark (free) plan for a single prototype. Delete
`smart_home/history` in the console if it grows during long tests.
