# Wiring

Pin numbers match `firmware/esp32/include/pins.h`.

Use a **common GND** between the ESP32, 5 V supply, sensors, and actuators.

```
ESP32 3.3 V  →  DHT VCC, OLED VCC, LDR top of divider (optional 3.3 V)
ESP32 5 V    →  PIR VCC, MQ-2 VCC, relay VCC, fan supply (or use a separate 5 V adapter)
ESP32 GND    →  every module GND
```

If the 5 V pin on the DevKit is weak, power PIR / MQ-2 / relay / fan from a USB 5 V adapter and join grounds only.

## Pin table

| Node | ESP32 GPIO | Module pin | Extra parts |
| ---- | ---------- | ---------- | ----------- |
| DHT data | **4** | DATA / OUT | 4.7–10 kΩ pull-up to 3.3 V if the breakout has none |
| PIR | **27** | OUT | — |
| LDR | **34** (input only) | divider midpoint | LDR to 3.3 V, 10 kΩ to GND |
| MQ-2 analog | **35** (input only) | A0 through divider | see below |
| LED light | **25** | LED anode via 220 Ω, cathode to GND | |
| Fan | **26** | transistor base via 1 kΩ | 2N2222 + 1N4007 |
| Relay IN | **32** | IN | VCC=5 V, GND common |
| Buzzer | **33** | + of active buzzer | − to GND; use a transistor if the buzzer is loud / 5 V |
| OLED SDA | **21** | SDA | |
| OLED SCL | **22** | SCL | |

## LDR divider

Firmware treats **higher ADC as more light**.

```
3.3 V --- LDR --- GPIO34 --- 10 kΩ --- GND
```

If your readings go the other way (bright room = low number), swap LDR and 10 kΩ, or invert the comparison in documentation and threshold thinking. The default code uses `light < light_threshold` as “dark”.

## MQ-2 analog — required divider

MQ-2 analog output can be up to **5 V**. ESP32 ADC pins are **3.3 V maximum**.

```
MQ-2 A0 --- 10 kΩ --- GPIO35 --- 10 kΩ --- GND
                 (midpoint)
```

Two equal resistors divide ~5 V to ~2.5 V. Do not skip this.

The heater needs a few minutes after power-on. Analog values drift during warm-up; do not set `gas_threshold` in that window.

Digital D0 on the MQ-2 is unused in this firmware. The onboard potentiometer on D0 does not change the analog path.

## Fan driver

GPIO cannot supply fan current.

```
GPIO26 -- 1 kΩ -- 2N2222 base
2N2222 emitter -- GND
2N2222 collector -- fan negative
fan positive -- 5 V
1N4007 across fan (cathode to 5 V, anode to collector)
```

## LED

```
GPIO25 -- 220 Ω -- LED anode
LED cathode -- GND
```

## Relay

Firmware assumes **active-LOW** (`RELAY_ACTIVE_LOW 1` in `pins.h`). If the LED on the relay module turns on when the ESP32 boots and the IN pin is HIGH, your module is active-LOW.

Switch only a **DC** load on the COM/NO terminals, for example a second LED with its own resistor on 5 V.

## Buzzer

Active buzzer: `+` to GPIO33 (or transistor), `−` to GND. HIGH = sound.

## OLED

0x3C I2C, 3.3 V, SDA=21, SCL=22. If `initDisplay()` prints “not found”, the rest of the system still runs.

## What not to wire

- No 220 V on the relay.
- No 5 V into GPIO 34/35 without the divider.
- No fan directly on GPIO 26.

## After wiring

1. USB connect, 115200 serial.
2. Confirm DHT numbers are not `nan`.
3. Wave a hand in front of the PIR.
4. Cover the LDR and watch ADC fall.
5. Only then enable Firebase.
