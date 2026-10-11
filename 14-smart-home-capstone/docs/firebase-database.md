# Firebase database schema

Root node: **`smart_home`**

Firmware macros are in `firmware/esp32/include/paths.h`.
The dashboard uses `window.FIREBASE_ROOT = "smart_home"`.

```
smart_home
├── sensors
│   ├── temperature     number  (°C)
│   ├── humidity        number  (%)
│   ├── motion          boolean
│   ├── light           number  (ADC 0–4095)
│   └── gas             number  (ADC 0–4095, after divider)
├── devices
│   ├── light           boolean
│   ├── fan             boolean
│   ├── relay           boolean
│   └── buzzer          boolean
├── settings
│   ├── mode            "auto" | "manual"
│   ├── temperature_threshold   number  (°C)
│   ├── light_threshold         number  (ADC)
│   ├── gas_threshold           number  (ADC)
│   └── motion_hold_ms          number  (milliseconds)
└── system
    ├── online          boolean
    └── last_update     number  (Firebase server timestamp, ms)
```

History (appended, not overwritten):

```
smart_home/history
└── {pushId}
    ├── temperature
    ├── humidity
    ├── motion
    ├── light
    ├── gas
    ├── light_on
    ├── fan_on
    └── timestamp
```

## Field meaning

| Field | Unit / type | Source |
| ----- | ----------- | ------ |
| `sensors/temperature` | °C | DHT22 |
| `sensors/humidity` | % RH | DHT22 |
| `sensors/motion` | true if PIR OUT is HIGH | HC-SR501 |
| `sensors/light` | 12-bit ADC | LDR divider |
| `sensors/gas` | 12-bit ADC | MQ-2 through 10 kΩ + 10 kΩ |
| `devices/*` | true = ON | GPIO state |
| `settings/mode` | `"auto"` or `"manual"` | dashboard (ESP32 defaults to auto) |
| `system/last_update` | ms since epoch | `Firebase.RTDB.setTimestamp` |

`light` and `gas` are **not** lux or ppm. Thresholds are ADC counts you tune
on your breadboard.

## Defaults

Written by firmware if `settings/mode` is missing, and listed in `firebase/seed.json`:

| Key | Default |
| --- | ------- |
| mode | `auto` |
| temperature_threshold | 30 |
| light_threshold | 1500 |
| gas_threshold | 1800 |
| motion_hold_ms | 10000 |

Tune `light_threshold` and `gas_threshold` after you see serial or dashboard
numbers in a normal room.

## Rules

See `firebase/database.rules.json`. Signed-in users can read and write the
`smart_home` tree. That is the classroom setup used by both the ESP32 and
the dashboard.

## Cleaning history

Long runs create many `history` children. Delete `smart_home/history` in the
console if the tree becomes large. The dashboard only listens to the last 30
nodes for display.
