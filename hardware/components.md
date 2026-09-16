# Components

Student-scale bill of materials. Substitutions are noted where a part is not unique.

**Do not connect 220 V / 110 V mains.** All loads in this model are low-voltage DC.

## Core

| Qty | Part | Chosen model | Why this one |
| --- | ---- | ------------ | ------------ |
| 1 | Microcontroller | ESP32-WROOM-32 30-pin DevKit | Wi-Fi on chip, 12-bit ADC, common in school labs |
| 1 | USB cable | USB-A or USB-C to the board | Power + serial upload |
| 1 | USB 5 V supply | Phone charger ≥ 1 A | Extra current for fan, relay, MQ-2 heater |

## Sensors

| Qty | Part | Chosen model | Notes |
| --- | ---- | ------------ | ----- |
| 1 | Temperature + humidity | **DHT22 (AM2302)** | Default in firmware. DHT11 works if you change `DHT_TYPE` in `include/pins.h`. DHT22 is a bit more accurate and still easy to wire. |
| 1 | Motion | **HC-SR501** PIR | Digital output. Adjust the two potentiometers (sensitivity, time) on the module. |
| 1 | Light | **LDR (photoresistor) + 10 kΩ** | Voltage divider into ADC pin 34. Not a calibrated lux meter. |
| 1 | Gas / smoke | **MQ-2** | Analog smoke/LPG/CO-ish reading. Heater needs 5 V. Analog output must be divided before the ESP32 ADC. See `wiring.md`. |

## Actuators (low voltage only)

| Qty | Part | Chosen model | Notes |
| --- | ---- | ------------ | ----- |
| 1 | Light | 5 mm LED + **220 Ω** resistor | Stands in for a room lamp |
| 1 | Fan | 5 V DC brushless fan, 25–40 mm | Driven by NPN transistor, not by the GPIO pin itself |
| 1 | Extra load switch | 1-channel **5 V relay module** | Switching a second LED or a small 5 V load. Active-LOW assumed (`RELAY_ACTIVE_LOW 1`) |
| 1 | Alarm | **Active** 3.3–5 V buzzer | On/off, not a passive piezo that needs PWM |

## Optional

| Qty | Part | Chosen model | Notes |
| --- | ---- | ------------ | ----- |
| 1 | Local screen | **SSD1306 128×64 OLED**, I2C, 0x3C | Optional. Set `ENABLE_OLED 0` in `config.h` if unused. |

## Discrete parts

| Qty | Part | Use |
| --- | ---- | --- |
| 1 | 4.7 kΩ or 10 kΩ | DHT data pull-up to 3.3 V (some modules already have it) |
| 1 | 10 kΩ | LDR divider |
| 2 | 10 kΩ | MQ-2 analog divider (5 V → ~2.5 V) |
| 1 | 1 kΩ | Transistor base resistor for the fan |
| 1 | NPN transistor | **2N2222** or **S8050** | Fan switch |
| 1 | Diode | **1N4007** | Flyback across the fan |
| 1 | Breadboard + jumper wires | Prototype wiring |
| 1 | Common ground jumper | ESP32 GND must share GND with 5 V peripherals |

## Power notes

- ESP32 logic is **3.3 V**. Do not feed 5 V into GPIO.
- MQ-2 heater and many relay modules want **5 V**.
- PIR modules are usually powered at 5 V; the output is typically 3.3 V, which is ESP32-safe.
- One USB port may brown-out when the fan and MQ-2 start. A separate 5 V supply with common GND is more stable.

## What this list is not

No mains wiring, no smart plugs on 220 V, no camera, no fingerprint sensor, no battery management IC. Those were left out because the hardware for this model does not include them.
