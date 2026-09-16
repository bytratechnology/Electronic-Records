# IoT-Based Smart Home Monitoring and Automation System

A student STEM project: an ESP32 prototype that monitors a model room, runs
simple automatic rules, and can be controlled from a web dashboard through
Firebase Realtime Database.

This is a **low-voltage classroom model**. It does not switch mains electricity
and it is not a commercial home-security product.

TODO: Add prototype photo (`images/prototype/prototype-front.jpg`)

TODO: Add circuit diagram (`images/circuit/circuit-diagram.jpg`)

TODO: Add dashboard screenshot (`images/dashboard/dashboard-sensors.png`)

---

## Project Overview

The system reads five inputs (temperature, humidity, motion, light, gas/smoke)
and drives four outputs (LED lamp, DC fan, relay, buzzer). Sensor values and
device states are stored under `smart_home/` in Firebase. A static HTML
dashboard displays the live data, switches Auto/Manual mode, and — in Manual
mode — toggles the outputs.

More detail: [docs/project-overview.md](docs/project-overview.md)

## Why I Built This Project

I wanted one project that I could wire, program, and explain: electronics on
the breadboard, C++ on the microcontroller, and a small cloud path to a
browser. A “smart home” is a familiar story, so I could spend the effort on
making the loop actually close (sense → decide → act → show) instead of on
a new metaphor.

## Problem

School rooms and model houses are often demonstrated with a single sensor
and an LED. Real IoT work is messier: several sensors, conflicting commands,
a network that drops, and a UI that must not fight the firmware. I wanted
to practise that full path at a scale I can power from USB.

## My Goals

- Read DHT, PIR, LDR, and MQ-2 on an ESP32 without freezing the rest of the code
- Keep automatic rules and manual overrides on the same outputs without them
  overwriting each other incorrectly
- Move live data and commands through Firebase Realtime Database
- Build a dashboard a teacher can operate in a demo
- Document pins, credentials, and tests so the build is reproducible

## What I Built

- Modular ESP32 firmware (sensors, devices, automation, cloud, optional OLED)
- Firebase schema, auth-based rules, and a seed tree
- A dashboard in HTML, CSS, and JavaScript (no extra JS framework)
- Hardware notes for a 5 V / 3.3 V prototype
- Portfolio documentation (architecture, testing sheet, learning journey)

I have not invented passing lab results. The test table in
[docs/testing.md](docs/testing.md) is marked **TODO** until I run it on the
desk.

## How It Works

1. The ESP32 samples sensors on a timer.
2. If `settings/mode` is `auto`, it applies threshold rules (fan, light+motion,
   gas buzzer).
3. If the mode is `manual`, it applies booleans written by the dashboard.
4. About every two seconds it uploads `sensors`, `devices`, and `system`
   (online flag + server timestamp).
5. About every minute it appends a history snapshot for later CSV export.
6. The dashboard listens to the same tree and writes `settings` (and
   `devices` in manual mode).

If Wi-Fi or Firebase is down, the firmware keeps reading sensors and can
still run automatic mode with the last settings in RAM. It retries the
network instead of rebooting in a loop.

## Hardware

Full list: [hardware/components.md](hardware/components.md)  
Wiring: [hardware/wiring.md](hardware/wiring.md)

| Role | Part |
| ---- | ---- |
| Controller | ESP32-WROOM 30-pin DevKit |
| Temperature / humidity | DHT22 (DHT11 optional) |
| Motion | HC-SR501 |
| Light | LDR + 10 kΩ divider |
| Gas / smoke | MQ-2 + 10 kΩ + 10 kΩ analog divider |
| Light output | LED + 220 Ω |
| Fan | 5 V DC fan via 2N2222 |
| Extra output | 5 V relay module (DC load only) |
| Alarm | Active buzzer |
| Optional UI | SSD1306 128×64 OLED (I2C) |

**GPIO:** DHT 4, PIR 27, LDR 34, MQ-2 35, LED 25, fan 26, relay 32, buzzer 33,
OLED SDA/SCL 21/22.

## Software

| Piece | Choice |
| ----- | ------ |
| Firmware | C++ / Arduino framework, PlatformIO (`firmware/esp32`) |
| Libraries | Adafruit DHT, Mobizt Firebase ESP client, Adafruit SSD1306 |
| Cloud | Firebase Authentication (email/password) + Realtime Database |
| Dashboard | HTML, CSS, JavaScript, Firebase compat SDK 10 |

I did not use a heavier front-end framework. The dashboard is a demo page,
not a product SPA.

## Key Features

- Live temperature, humidity, motion, light ADC, gas ADC
- Auto and Manual modes
- Thresholds stored in Firebase (`temperature_threshold`, `light_threshold`,
  `gas_threshold`, `motion_hold_ms`)
- Dashboard controls for light, fan, relay, buzzer in Manual mode
- Gas alert on the buzzer (Auto) and on the dashboard
- History log + CSV download
- Optional local OLED
- Credentials kept out of git (`config.example.h`, `firebase-config.example.js`)

## Automatic Mode

See [docs/automatic-control.md](docs/automatic-control.md).

- Temperature above threshold → fan ON
- Dark **and** recent motion → LED ON
- Gas above threshold → buzzer ON
- Relay is not driven by auto rules (spare DC output)

## Manual Mode

The dashboard sets `settings/mode` to `manual` and writes `devices/*`.
Automatic rules do not override those commands until the mode is `auto`
again.

## System Architecture

```
Sensors → ESP32 → Wi-Fi → Firebase RTDB → Dashboard
Dashboard → Firebase RTDB → ESP32 → LED / fan / relay / buzzer
```

[docs/system-architecture.md](docs/system-architecture.md)  
[docs/firebase-database.md](docs/firebase-database.md)

## My Contribution

On this project I:

- **Designed** the GPIO map, Firebase tree, and Auto/Manual split
- **Wrote** the firmware in separate modules instead of one large `loop()`
- **Documented** breadboard wiring, including the MQ-2 divider and fan transistor
- **Integrated** Firebase Auth + RTDB on both the ESP32 and the dashboard
- **Built** the dashboard (layout, live listeners, threshold form, CSV export)
- **Prepared** a test sheet to fill during bring-up

Physical soldering/breadboard assembly, Serial debugging on the real board,
and ticking tests to PASS/FAIL happen when the kit is in front of me. I am
not listing those as finished results in this README until they are.

## Challenges I Faced

I keep a lab-style log in [docs/challenges.md](docs/challenges.md). It is a
template until I fill it with real faults (wrong DHT type, active-low relay,
Firebase token not ready, and so on). I would rather leave it empty than
invent a debugging story.

## How I Solved Them

Each challenge block has space for what I tried, what failed, and the fix.
The design already encodes a few lessons I did not want to re-learn on the
bench: ADC pins 34/35 are input-only, MQ-2 analog is divided, Wi-Fi connect
has a timeout, OLED failure is non-fatal.

## What I Learned

- Electronics: 3.3 V logic vs 5 V modules, dividers, a transistor as a switch
- Embedded systems: timers, cached DHT reads, reconnect without a watchdog reset storm
- IoT: a shared JSON tree as the contract between firmware and UI
- Process: pin headers, wiring notes, and dashboard paths must stay identical

[docs/learning-journey.md](docs/learning-journey.md)

## Testing

Procedure and empty results table: [docs/testing.md](docs/testing.md)

Until I run the suite, every status is **TODO**. That includes Wi-Fi,
Firebase, Auto/Manual, gas alert, and motion-hold tests.

## Results

**Not yet filled from hardware.** After bring-up I will record:

- Typical DHT readings in the room
- LDR ADC in light vs covered
- MQ-2 ADC after warm-up vs a safe stimulus
- Whether Auto fan/light/gas matched the rule table
- Whether Manual blocked those rules

I will not put example numbers here just to make the README look complete.

## Limitations

- LDR and MQ-2 are relative ADC values, not lux or ppm
- PIR only sees motion in a cone; an empty still person may time out
- Relay is a model output, not a household branch circuit
- Firebase free-plan history will grow if the board runs for days
- The dashboard user password lives in a local JS file; rules are classroom-grade
- No battery, no OTA, no mobile app

## Future Improvements

Listed as **future work** in [docs/future-development.md](docs/future-development.md):
mobile app, energy monitoring, scheduling, weather API, voice, machine
learning, occupancy prediction. None of those are in the current firmware.

The next realistic firmware step is fan hysteresis and a cap on history
length.

## Academic Interests

This project is the kind of work I want to continue in engineering or
computer science: measurement, embedded software, and networked systems,
kept at a scale I can test myself. Portfolio wording:
[docs/student-portfolio.md](docs/student-portfolio.md)

## Project Structure

```
smart-home-iot/
├── README.md
├── LICENSE
├── .gitignore
├── SECURITY.md
├── firmware/esp32/          PlatformIO firmware
├── dashboard/               HTML/CSS/JS UI
├── hardware/                parts + wiring
├── firebase/                rules + seed JSON
├── docs/                    architecture and portfolio notes
└── images/                  photo placeholders
```

## Setup

Do not commit secrets. Read [SECURITY.md](SECURITY.md) first.

### 1. Firmware

```bash
cp firmware/esp32/config.example.h firmware/esp32/include/config.h
```

Edit Wi-Fi and Firebase fields. Then, from `firmware/esp32/`:

```bash
pio run --target upload
pio device monitor
```

Library and Arduino IDE notes: [firmware/esp32/README.md](firmware/esp32/README.md)

### 2. Firebase

Follow [firebase/README.md](firebase/README.md): enable Email/Password, create
Realtime Database, paste [firebase/database.rules.json](firebase/database.rules.json).

### 3. Dashboard

```bash
cp dashboard/firebase-config.example.js dashboard/firebase-config.js
cd dashboard
python3 -m http.server 8080
```

Open http://localhost:8080

### 4. Hardware

Wire using [hardware/wiring.md](hardware/wiring.md). Power MQ-2 and the fan
from 5 V with common GND. Never connect 220 V to the relay.

## Author

**[Student Name]**  
High-school STEM student building this prototype as a study-abroad portfolio
project.

Replace this line with your name, school, year, and a contact email you are
willing to publish.

## License

[MIT](LICENSE) — replace the copyright name in `LICENSE` when you publish.
