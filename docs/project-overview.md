# Project overview

This repository is a classroom model of a small smart home. An ESP32 reads
temperature, humidity, motion, light, and gas; it drives an LED, a 5 V fan,
a relay, and a buzzer; and it shares state with a web dashboard through
Firebase Realtime Database.

It is a **student STEM project**, not a commercial product and not a
mains-powered installation.

## What the system does

1. Measure temperature and humidity (DHT22).
2. Detect motion (HC-SR501).
3. Measure a relative light level (LDR).
4. Measure a relative gas/smoke level (MQ-2 analog).
5. Turn an LED “light” on and off.
6. Turn a small DC fan on and off.
7. Switch an extra low-voltage load through a relay.
8. Sound a buzzer for a gas alert (auto) or on command (manual).
9. Upload readings to Firebase.
10. Show readings on a dashboard and accept commands from that dashboard.
11. Run **automatic** rules from thresholds stored in Firebase.
12. Run **manual** commands that those rules must not override.
13. Append a snapshot about once a minute for later CSV export.

## What it does not do

- It does not switch 220 V lighting or appliances.
- It does not measure calibrated lux, ppm, or energy (kWh).
- It does not include a camera, voice assistant, or phone app.
- It does not claim machine learning or occupancy prediction. Those ideas
  are listed under future work only.

## Design limits I accepted

I used parts that are common, cheap, and 3.3 V / 5 V safe. The LDR and MQ-2
give relative ADC numbers, not SI units. That is enough to demonstrate
sensing, thresholds, and closed-loop control on a breadboard.

## Repository map

| Path | Content |
| ---- | ------- |
| `firmware/esp32/` | Device firmware |
| `dashboard/` | Web UI |
| `hardware/` | Parts list and wiring |
| `firebase/` | Rules and seed JSON |
| `docs/` | Architecture, testing, portfolio notes |
| `images/` | Photo placeholders until the build is photographed |
