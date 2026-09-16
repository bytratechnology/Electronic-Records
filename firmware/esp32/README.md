# ESP32 Firmware

Firmware for the smart-home prototype. It reads sensors, applies automatic or
manual control, and syncs with Firebase Realtime Database.

## What this folder contains

```
esp32/
├── platformio.ini
├── config.example.h
├── include/          headers
└── src/              implementation
    ├── main.cpp           setup() / loop()
    ├── sensors.cpp        DHT, PIR, LDR, MQ-2
    ├── devices.cpp        LED, fan, relay, buzzer
    ├── automation.cpp     auto and manual rules
    ├── cloud.cpp          Wi-Fi + Firebase
    ├── display_oled.cpp   optional SSD1306
    └── types.cpp
```

`cloud.cpp` is named that way on purpose. The Firebase library already
provides `Firebase.h`, which collides with a project file named `firebase.h`
on macOS.

## Modules

| Function | File | Role |
| -------- | ---- | ---- |
| `readTemperature()` / `readHumidity()` | `sensors.cpp` | DHT22 (or DHT11) |
| `readMotion()` | `sensors.cpp` | HC-SR501 |
| `readLight()` | `sensors.cpp` | LDR analog |
| `readGas()` | `sensors.cpp` | MQ-2 analog |
| `controlLight()` / `controlFan()` / `controlRelay()` / `controlBuzzer()` | `devices.cpp` | outputs |
| `automaticControl()` / `manualControl()` | `automation.cpp` | mode logic |
| `connectWiFi()` / `connectFirebase()` / `updateFirebase()` / `readFirebaseCommands()` | `cloud.cpp` | network |

The main loop does not contain the control rules. It only sequences the modules.

## GPIO (ESP32-WROOM 30-pin)

| Pin | Signal |
| --- | ------ |
| 4 | DHT data |
| 27 | PIR OUT |
| 34 | LDR analog |
| 35 | MQ-2 analog (through voltage divider) |
| 25 | LED (light) |
| 26 | Fan transistor |
| 32 | Relay IN |
| 33 | Buzzer |
| 21 / 22 | OLED SDA / SCL |

Full wiring is in `hardware/wiring.md`.

## Prepare config.h

```bash
cp firmware/esp32/config.example.h firmware/esp32/include/config.h
```

Edit `include/config.h` with your Wi-Fi SSID, password, Firebase URL, API key,
and the email/password of a Firebase Authentication user.

Never commit `config.h`.

If you do not have an OLED module, set `#define ENABLE_OLED 0`.

If you use DHT11 instead of DHT22, change `DHT_TYPE` in `include/pins.h`.

## Build with PlatformIO (recommended)

1. Install [PlatformIO](https://platformio.org/) (VS Code / Cursor extension or CLI).
2. Connect the ESP32 by USB.
3. From `firmware/esp32/`:

```bash
pio run
pio run --target upload
pio device monitor
```

Hold BOOT if the upload port is not found, then tap EN / RESET.

## Build with Arduino IDE

1. Install ESP32 board support (Espressif).
2. Install libraries:
   - DHT sensor library (Adafruit)
   - Adafruit Unified Sensor
   - Adafruit SSD1306
   - Adafruit GFX
   - Firebase Arduino Client Library for ESP8266 and ESP32 (Mobizt)
3. Create a sketch folder and copy `include/*.h` plus `src/*.cpp` into it.
   Rename `main.cpp` to `smart_home.ino`, or keep a thin `.ino` that includes
   the same setup/loop.
4. Copy `config.example.h` to `config.h` in the sketch folder.
5. Select board **ESP32 Dev Module**, upload speed 921600, serial 115200.

## Offline behaviour

If Wi-Fi or Firebase drops:

- The firmware does not reboot in a loop.
- Sensor reads continue.
- Automatic mode still uses the last settings held in RAM.
- Uploads resume after `ensureFirebase()` succeeds again.

## Serial log

Open the monitor at **115200 baud**. You should see Wi-Fi IP, Firebase token
status, and DHT failures if the sensor wiring is wrong.

MQ-2 analog values are unstable for the first 1–3 minutes while the heater
warms up. Do not tune `gas_threshold` during that window.
