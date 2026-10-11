# Learning Journey

The path through the fourteen projects. Each step names the one thing I
had to understand before the next project could work.

The dates are placeholders. Replace each TODO with the month I really
built that project.

## Level 1 — Basic electronics

### 01 · Temperature and humidity monitor — TODO: date

The first complete system: sensor in, display out.

**What I learned:** a sensor has rules (the DHT cannot be read faster than
every two seconds), and a program should say “sensor error” instead of
showing an old number.

### 02 · Gas leak alarm — TODO: date

The first sensor whose number means nothing on its own.

**What I learned:** measure a baseline first, then decide relative to it.
Two thresholds (hysteresis) stop an alarm from chattering.

### 03 · Fire alarm — TODO: date

The first project with memory: the alarm stays on after the cause is gone.

**What I learned:** drawing the states on paper before writing code. A
button needs debouncing.

### 04 · Motion-activated light — TODO: date

The first project built entirely around time.

**What I learned:** `millis()` instead of `delay()`, so the program can
watch the sensor while it waits. PWM for a smooth fade.

### 05 · Ambient-light automatic lamp — TODO: date

The project that taught me that “works on my desk” is not “works”.

**What I learned:** why a lamp flickers at dusk and three different ways
to stop it. An actuator can disturb its own sensor.

## Level 2 — IoT

### 06 · Online weather station — TODO: date

Moving from Arduino Uno to ESP32, and from an LCD to a browser.

**What I learned:** the ESP32 runs at 3.3 V, not 5 V. A web page and the
data behind it are separate things. Wi-Fi code needs a timeout.

### 07 · Internet gas alert — TODO: date

The first message sent to a phone.

**What I learned:** how to call a web API over HTTPS. The buzzer must never
wait for the Internet. A 5 V analog output needs a divider before an ESP32
pin.

### 08 · Real-time fire alert — TODO: date

Changing from “the browser asks” to “the device tells”.

**What I learned:** MQTT publish / subscribe, retained messages, and the
last will that reports a device as offline when it loses power.

### 09 · Web-controlled smart light — TODO: date

The first time a browser changed something physical.

**What I learned:** when a button and a web page control the same lamp,
the state must live in one place. Never trust a value just because it came
from my own page.

### 10 · IoT plant watering — TODO: date

The first feedback loop, and the first motor.

**What I learned:** soil reacts slowly, so pump briefly and wait. Plan for
the sensor failing. Give a motor its own power supply.

### 11 · RFID attendance — TODO: date

The first project that handles information about people.

**What I learned:** SPI wiring, real time from NTP, and putting a queue
between a fast reader and a slow network. Personal data needs care before
anything is published.

### 12 · Web app device control — TODO: date

Remote control with a login.

**What I learned:** the difference between what the user wants and what
the device has done. A login page protects nothing without database rules.

### 13 · Cloud data logging — TODO: date

Keeping data instead of only showing it.

**What I learned:** the time of a measurement belongs to the measurement,
not to the upload. Delete from the queue only after the server confirms.

### 14 · Smart home capstone — TODO: date

Everything together: five sensors, four outputs, automatic rules, manual
override, cloud dashboard.

**What I learned:** a large program has to be split into modules with
clear jobs. The pin map, the firmware, the database paths, and the
dashboard are one design and must agree.

## Patterns I now use by default

| Pattern | First used | Used again in |
| ------- | ---------- | ------------- |
| Non-blocking timing with `millis()` | 01 | every project |
| Hysteresis | 02 | 05, 07 |
| State machine | 02 | 03, 04, 07, 10 |
| Network with timeout and retry | 06 | 07–14 |
| Local function first, network second | 07 | 08, 09, 10, 14 |
| Queue between input and upload | 11 | 13 |
| Secrets in a gitignored file | 06 | 07–14 |
| Written test checklist | 01 | every project |

## What I would do differently

TODO: Fill in after building. Two or three honest points, for example a
project I would redesign, a mistake I repeated, or a tool I wish I had
learned earlier.
