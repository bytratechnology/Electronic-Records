# Testing

Fill **Actual Result** and **Status** when you run each test on the hardware.
Do not invent passing results.

Status values: `TODO` · `PASS` · `FAIL`

Default thresholds assumed unless you note a change:
temperature 30 °C, light 1500, gas 1800, motion hold 10 000 ms.

## Test log

| Test | Steps | Expected Result | Actual Result | Status |
| ---- | ----- | --------------- | ------------- | ------ |
| Sensor Test — DHT | Power the board, wait 2 s, read Serial or dashboard | Temperature and humidity are finite numbers, not NaN | | TODO |
| Sensor Test — PIR | Walk in front of HC-SR501 | `motion` becomes true, then false after the module delay | | TODO |
| Sensor Test — LDR | Cover the LDR, then shine a lamp | ADC falls when covered, rises when lit (with the documented divider) | | TODO |
| Sensor Test — MQ-2 | Wait 3 min after power, then hold a butane lighter *unlit* near the sensor or use a safe alcohol wipe at a distance | ADC rises compared with room baseline. Do not spray flammable gas at the heater. | | TODO |
| Wi-Fi Test | Boot with correct `config.h` | Serial shows an IP before the 20 s timeout | | TODO |
| Wi-Fi Test — wrong password | Temporarily wrong password | Firmware keeps running sensors; retries Wi-Fi; no crash loop | | TODO |
| Firebase Test | Valid API key, URL, email/password, rules | `smart_home/sensors` updates; `system/last_update` moves | | TODO |
| Firebase Test — disconnect | Turn off the router after a successful link | Auto rules still move the fan/LED; uploads resume when the network returns | | TODO |
| Relay Test | Manual mode, toggle Relay ON/OFF | Relay click + load LED follows. No mains connected. | | TODO |
| Dashboard Test | `python3 -m http.server 8080`, sign-in config | Cards match Firebase; mode buttons write `settings/mode` | | TODO |
| Automatic Mode Test — fan | Auto ON, warm DHT above threshold (hand / indoor heat) | Fan ON above threshold, OFF when it falls back | | TODO |
| Automatic Mode Test — light | Darken LDR and trigger PIR | LED ON while occupied and dark; OFF when bright or hold time ends | | TODO |
| Manual Mode Test | Set MANUAL, toggle Light and Fan | Outputs follow buttons; heating the DHT does not force the fan | | TODO |
| Gas Alert Test | Auto mode, raise MQ-2 ADC above threshold | Buzzer ON, dashboard shows Alert | | TODO |
| Motion Detection Test | Auto, dark room, wave hand | LED ON; stays ON for about `motion_hold_ms` after PIR drops | | TODO |
| OLED Test (if fitted) | Boot with OLED wired | Text shows temp/mode; missing OLED does not halt firmware | | TODO |
| History Test | Run > 1 minute connected | New child under `smart_home/history`; CSV download contains rows | | TODO |

## Notes (optional)

Date:

Firmware version / commit:

Room temperature during test:

LDR ADC in room light:

MQ-2 ADC after warm-up:

Anything that failed and how you followed up:
