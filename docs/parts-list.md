# Parts List

Every component used in projects 01–13. The capstone has its own detailed
list in [14-smart-home-capstone/hardware/components.md](../14-smart-home-capstone/hardware/components.md).

Most parts are reused between projects, so one kit covers the whole
portfolio. All loads are low-voltage DC. **No mains electricity.**

## Controllers

| Qty | Part | Used in |
| --- | ---- | ------- |
| 1 | Arduino Uno R3 + USB cable | 01–05 |
| 1 | ESP32-WROOM-32 DevKit (30-pin) + USB cable | 06–14 |

## Sensors

| Qty | Part | Interface | Used in |
| --- | ---- | --------- | ------- |
| 1 | DHT11 module | Digital, one wire | 01 |
| 1 | DHT22 (AM2302) module | Digital, one wire | 06, 08, 13, 14 |
| 1 | MQ-2 gas sensor module | Analog | 02, 07, 14 |
| 1 | IR flame sensor module (KY-026 type) | Analog | 03, 08 |
| 1 | HC-SR501 PIR motion sensor | Digital | 04, 14 |
| 1 | LDR (photoresistor) | Analog, via divider | 05, 13, 14 |
| 1 | Capacitive soil moisture sensor v1.2 | Analog | 10 |
| 1 | RC522 RFID reader + cards / key fobs | SPI, 3.3 V only | 11 |

## Outputs and displays

| Qty | Part | Used in |
| --- | ---- | ------- |
| 1 | LCD 16×2 with I2C backpack | 01 |
| 1 | Active buzzer (3.3–5 V) | 02, 03, 07, 08, 11, 14 |
| 4 | LED 5 mm (red, green, white / yellow) | most projects |
| 1 | 1-channel 5 V relay module or MOSFET module | 10, 14 |
| 1 | 4-channel 5 V relay module | 12 |
| 1 | 3–6 V mini submersible pump + tube | 10 |

## Passive parts and tools

| Qty | Part | Used for |
| --- | ---- | -------- |
| 6 | 220 Ω resistor | LED current limiting |
| 4 | 10 kΩ resistor | LDR divider, MQ-2 divider, DHT pull-up |
| 1 | 10 kΩ potentiometer | Set point in 05 |
| 2 | Push button | 03, 09 |
| 1 | Breadboard (830 points) | all |
| — | Jumper wires (M-M, M-F) | all |
| 1 | 5 V USB supply or power bank | Pump in 10, extra current for relays |
| 1 | Multimeter | Checking voltages before connecting a pin |

## Software and services

| Tool | Used in | Cost |
| ---- | ------- | ---- |
| PlatformIO (or Arduino IDE) | all | Free |
| Telegram Bot API | 07 | Free |
| MQTT broker (public test broker or own) | 08 | Free |
| Google Sheets + Apps Script | 11 | Free |
| Firebase Authentication + Realtime Database | 12, 13, 14 | Free plan |

## Voltage notes

| Part | Supply | Signal to controller |
| ---- | ------ | -------------------- |
| MQ-2 | 5 V | Up to 5 V analog → **divider required on ESP32** |
| HC-SR501 | 5 V | 3.3 V digital, safe for ESP32 |
| Flame sensor | 3.3 V on ESP32 | Stays within ADC range |
| RC522 | **3.3 V only** | 3.3 V |
| Relay modules | 5 V | Usually active-LOW input |
| Pump | Separate 5 V | Never from a controller pin |
