# Learning journey

This page is the path through the project, not a list of marketing features.
Each step names something I had to understand before the next step worked.

## 1. Idea

I wanted a single breadboard that could *sense* a room, *act* on that data,
and *show* the same data on a laptop. A smart-home model is familiar to
admissions readers and still small enough to finish.

**What I learned:** a useful IoT project is a loop (sense → decide → act →
report), not a pile of sensors.

## 2. Research

I compared ESP8266 and ESP32, DHT11 and DHT22, and whether a local web
server on the chip would be enough. ESP32 has more ADC channels. Firebase
Realtime Database fits a two-way dashboard better than a page hosted only
on the microcontroller.

**What I learned:** how to read datasheets enough to see voltage, pin type,
and “this output is analog, that one is digital.”

## 3. Hardware selection

I chose low-voltage stand-ins for lamps and fans so the model never needs
mains electricity. MQ-2 is a typical school gas sensor; I also had to plan
a divider because the analog output is not automatically 3.3 V-safe.

**What I learned:** school “sensor modules” still have power and logic
levels that can damage a board if you wire them like 5 V Arduino examples.

## 4. Circuit design

I wrote a GPIO map first, then the wiring table, then the firmware `#define`
list so the three documents could not drift.

**What I learned:** input-only RTC pins (34–39) are fine for ADC and a
mistake for a fan transistor.

## 5. ESP32 programming

I split sensors, devices, automation, and cloud into separate files so
`loop()` only sequences work. DHT temperature and humidity share one read
with a 2 s cache.

**What I learned:** blocking `while (WiFi.status() != CONNECTED)` forever
is a poor pattern; a timeout lets the house logic still run.

## 6. Sensor testing

Each sensor has a different “it is working” check: DHT must not return NaN,
PIR is a digital edge, LDR is a relative ADC, MQ-2 needs warm-up.

**What I learned:** you cannot tune `gas_threshold` in the first minute
after power-up.

## 7. Firebase integration

The firmware signs in with email/password, updates a JSON node, and reads
settings on a poll. History uses `push` so old samples are not overwritten.

**What I learned:** API keys in a web app are not a substitute for database
rules; both the ESP32 and the browser still need a user.

## 8. Dashboard development

The UI is static HTML/CSS/JS and the Firebase compat SDK. Manual toggles
are disabled in auto mode so the page cannot fight the firmware.

**What I learned:** a dashboard is part of the control system, not only a
display. Who is allowed to write `devices/light` matters.

## 9. Debugging

Typical tools on this project: USB serial at 115200, the Firebase console
tree, and the browser debug console.

**What I learned:** say whether a bug is wiring, firmware, rules, or the
dashboard before changing all four at once.

## 10. System testing

End-to-end means: change a threshold on the page → see the ESP32 react →
see the device card update. The checklist lives in `docs/testing.md` and
starts as TODO until I run it on the desk.

**What I learned:** automatic and manual mode are different test cases, not
one “it works” sentence.

## 11. Improvements after the first wiring

Planned modest improvements (only after the base path works): OLED if the
module is available, CSV export from history, presence based on a stale
`last_update`.

**What I learned:** extra features are easy to add and easy to make the
demo flaky; finish the loop first.

## 12. Final system

The intended finished state is: breadboard powered from USB, dashboard on
localhost, auto fan/light/gas rules, manual override, and photos in
`images/` of that real setup.

Until those photos and the test table are filled, the repository is the
design and source code of the project, not a claim that every test passed.
