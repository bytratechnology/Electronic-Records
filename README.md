# Electronics and IoT Project Portfolio

Fourteen projects that go from a single sensor on an Arduino to a
cloud-connected smart-home model on an ESP32. Each project is in its own
folder and is written up as a short engineering report: problem and
objectives, background, wiring and logic diagrams, build steps, code
walkthrough, design calculations, test plan, results, troubleshooting,
limitations, and references.

**[Student Name]** · [School], [City, Country] · Class of [Year]

Replace the line above with your own details and a contact email you are
willing to publish.

TODO: Add one photo of the workbench or the finished capstone (`docs/images/overview.jpg`)

---

## About this portfolio

I built these projects to learn how a physical measurement becomes a
decision, and how that decision reaches a person who is somewhere else.
The projects are ordered the way I learned: every folder adds one or two
new ideas to the ones before it.

The portfolio has two levels:

- **Level 1 — Basic electronics (projects 01–05).** Arduino Uno, one sensor,
  one output, no network. The focus is reading sensors correctly and making
  stable decisions.
- **Level 2 — IoT (projects 06–14).** ESP32 with Wi-Fi. The focus is
  connecting devices to browsers, phones, and cloud services without making
  the device depend on the network to do its main job.

## Projects

### Level 1 — Basic electronics

| # | Project | What it does | Main idea learned |
| - | ------- | ------------ | ----------------- |
| 01 | [Temperature and Humidity Monitor](01-temperature-humidity-monitor/) | DHT sensor on an LCD with min / max | Non-blocking timing, I2C, handling failed reads |
| 02 | [Gas Leak Alarm](02-gas-leak-alarm/) | MQ-2 sensor with buzzer and LEDs | Warm-up, baseline calibration, hysteresis |
| 03 | [Fire Alarm](03-fire-alarm/) | Flame sensor with latching alarm and silence button | State machines, debouncing, confirmation samples |
| 04 | [Motion-Activated Light](04-motion-activated-light/) | PIR switches a lamp with hold time and fade | Timers, PWM |
| 05 | [Ambient-Light Automatic Lamp](05-ambient-light-auto-lamp/) | LDR switches a lamp at dusk without flicker | Voltage dividers, smoothing, dwell time |

### Level 2 — IoT

| # | Project | What it does | Main idea learned |
| - | ------- | ------------ | ----------------- |
| 06 | [Online Weather Station](06-online-weather-station/) | ESP32 serves live readings and a chart as a web page | HTTP server, JSON API, ring buffer |
| 07 | [Internet Gas Alert](07-internet-gas-alert/) | Gas alarm that also sends a Telegram message | Calling a web API over HTTPS, retry, local-first safety |
| 08 | [Real-Time Fire Alert](08-realtime-fire-alert/) | Fire alarm pushed to a browser over MQTT | Publish / subscribe, retained messages, last will |
| 09 | [Web-Controlled Smart Light](09-web-controlled-smart-light/) | Dimmable lamp with web page, button, and timer | REST-style control, single source of truth, input validation |
| 10 | [IoT Plant Watering](10-iot-plant-watering/) | Soil sensor and pump with web settings | Feedback control, calibration, safety limits, saved settings |
| 11 | [RFID Attendance](11-rfid-attendance/) | Card taps logged to a Google Sheet | SPI, NTP time, upload queue, data privacy |
| 12 | [Web App Device Control](12-web-app-device-control/) | Four relays controlled from anywhere, with login | Desired vs reported state, authentication, streaming |
| 13 | [Cloud Data Logging](13-cloud-data-logging/) | Sensor log in the cloud with chart and CSV | Store-and-forward, timestamps, database queries |
| 14 | [Smart Home Capstone](14-smart-home-capstone/) | Five sensors, four outputs, auto / manual modes, dashboard | Integrating everything into one modular system |

## How the projects build on each other

![Roadmap showing how the fourteen projects build on each other](docs/images/roadmap.svg)

Each arrow means “reuses and extends”. Every Level 1 project returns in
Level 2 with a network connection, and everything meets in the capstone.

## Skills demonstrated

| Area | Where to look |
| ---- | ------------- |
| Circuits: dividers, pull-ups, level matching 5 V ↔ 3.3 V, driving loads | 02, 05, 07, 10, 14 |
| Sensors: digital, analog, I2C, SPI, calibration | 01, 02, 05, 10, 11 |
| Embedded C++: non-blocking timing, state machines, debouncing | 02, 03, 04, 09 |
| Control: hysteresis, feedback, safety limits | 02, 05, 10 |
| Networking: HTTP server and client, JSON, HTTPS, MQTT, NTP | 06, 07, 08, 09, 11 |
| Cloud: authentication, database rules, real-time sync, queries | 12, 13, 14 |
| Web front end: HTML, CSS, JavaScript without a framework | 06, 08, 09, 10, 12, 13, 14 |
| Reliability: reconnect, retry, queues, working offline | 07, 08, 11, 13 |
| Engineering practice: documentation, test plans, secrets handling, stated limits | every folder |

## Status and honesty

- **Source code:** complete. All fourteen firmware projects compile
  (Arduino Uno for 01–05, ESP32 for 06–14).
- **Hardware tests:** each project README has a test checklist. A row says
  **TODO** until I have run that test on the real circuit and written down
  what happened.
- **Diagrams:** every project has a wiring diagram and a logic diagram
  (`images/wiring.svg`, `images/logic.svg`) drawn from the pin definitions
  in the code.
- **Photos and measurements:** placeholders are marked TODO. I add only real
  photos and real numbers.

These are learning models on USB power. None of them is a certified gas,
fire, or security device, and none switches mains electricity.

## Repository layout

```
.
├── README.md                    this page
├── SECURITY.md                  secrets and bench safety
├── docs/
│   ├── student-portfolio.md     one-page summary for teachers and admissions readers
│   ├── technical-report.md      cross-project engineering analysis
│   ├── application-guide.md     how I use this portfolio in applications
│   ├── evidence-checklist.md    photos, videos and measurements to collect
│   ├── learning-journey.md      what each project taught me, in order
│   └── parts-list.md            every component used
├── 01-temperature-humidity-monitor/
│   ├── README.md                purpose, wiring, logic, engineering notes, tests, limits
│   ├── firmware/                PlatformIO project (src/main.cpp)
│   └── images/                  wiring.svg, logic.svg, photos
├── …
└── 14-smart-home-capstone/
```

Projects with a browser part also have a `monitor/`, `web-app/`, or
`viewer/` folder; project 11 has `apps-script/`.

## Running a project

Every firmware folder is a [PlatformIO](https://platformio.org/) project:

```bash
cd 06-online-weather-station/firmware
mkdir -p include && cp config.example.h include/config.h   # Level 2 only; then edit
pio run --target upload
pio device monitor
```

The code also works in the Arduino IDE: copy `src/main.cpp` into a sketch,
put `config.h` (and `page.h` where present) next to it, and install the
libraries listed in `platformio.ini`.

Read [SECURITY.md](SECURITY.md) before adding any password.

## More

- [One-page portfolio summary](docs/student-portfolio.md)
- [Technical report](docs/technical-report.md)
- [Application guide](docs/application-guide.md)
- [Evidence checklist](docs/evidence-checklist.md)
- [Learning journey](docs/learning-journey.md)
- [Parts list](docs/parts-list.md)

## License

[MIT](LICENSE) — replace the copyright name in `LICENSE` when you publish.
