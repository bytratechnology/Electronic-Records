# Student portfolio note

A short page for a teacher, counsellor, or admissions officer who will not
open every source file.

### Project

**IoT-Based Smart Home Monitoring and Automation System** — a low-voltage
ESP32 model that measures temperature, humidity, motion, light, and gas,
controls an LED, a DC fan, a relay, and a buzzer, and uses Firebase plus a
browser dashboard for live data and commands.

### My motivation

I wanted one project that combined electronics, embedded programming, and a
network service, because those are the skills I hope to study further. A
house-scale story is easy to explain; keeping it on 5 V keeps it honest.

### What I wanted to learn

- How to read sensors on an ESP32 without blocking forever
- How automatic control and a human override can share the same outputs
- How a cloud database can be a two-way bus, not only a logger
- How to document a build so someone else can reproduce it

### What I built

- Modular firmware (`sensors`, `devices`, `automation`, `cloud`)
- A Firebase schema under `smart_home/`
- A static dashboard for sensors, mode, thresholds, and history
- Wiring and safety notes for a school prototype

Hardware assembly, bring-up, and the filled test table happen on the
physical kit. Until those photos exist, this write-up describes the system
I designed and programmed, not a staged product launch.

### Technical skills

| Area | In this project |
| ---- | ---------------- |
| Electronics | Voltage dividers, transistor fan driver, 3.3 V vs 5 V |
| Embedded systems | GPIO, ADC, non-blocking timers, optional I2C OLED |
| Programming | C++ modules, a small JS client |
| IoT | ESP32 Wi-Fi, reconnect without crash loops |
| Cloud / database | Firebase Auth + Realtime Database paths and rules |
| Automation | Threshold rules with occupancy hold |
| Debugging | Serial, Firebase console, browser tools (once the board is live) |
| Independent learning | Library docs, datasheets, splitting a demo into files |

### Problem solving

The design answers a few concrete constraints: no mains power, ADC pins that
cannot drive loads, MQ-2 analog voltage, and auto/manual that must not
fight each other. The code and `hardware/wiring.md` are the record of those
choices.

### Challenges

See `docs/challenges.md`. That file starts as a template so I do not invent
bench problems I have not had yet.

### Learning journey

See `docs/learning-journey.md`.

### Final result

**Intended result:** a working breadboard and a dashboard that stay in sync.
**Current repository result:** complete firmware, dashboard, and documentation
ready to flash and wire.

### What I learned

A connected device is a system: pin map, firmware, security rules, and UI
have to agree. If they disagree, the demo looks like “Wi-Fi is broken”
when the real bug is a path name or an active-low relay.

### Future goals

I would like to add hysteresis, cleaner history retention, and later a
phone client — listed as future work, not as if they already shipped.

### Connection to my academic interests

This project sits at the overlap of physics (sensors), computing
(embedded + cloud), and design (a model people can operate). That is the
direction I want in university-level engineering or computer science,
without pretending the breadboard is industrial building automation.
