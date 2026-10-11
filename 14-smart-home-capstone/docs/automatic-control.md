# Automatic and manual control

Implementation: `firmware/esp32/src/automation.cpp`.
Thresholds: `smart_home/settings` (Firebase), not literals inside `loop()`.

## Automatic mode (`settings/mode = "auto"`)

| Condition | Action |
| --------- | ------ |
| DHT read is valid **and** temperature **>** `temperature_threshold` | Fan ON |
| otherwise | Fan OFF |
| light ADC **<** `light_threshold` **and** occupied | LED ON |
| otherwise | LED OFF |
| gas ADC **>** `gas_threshold` | Buzzer ON |
| otherwise | Buzzer OFF |
| relay | **Not** changed by auto rules |

**Occupied** means PIR is HIGH now, or it was HIGH within `motion_hold_ms`
(default 10 s). That matches the idea “turn the light on if it is dark and
someone is there,” without flickering every time the PIR retriggers.

The ESP32 writes `devices/*` back to Firebase so the dashboard shows what
the rules actually did. Dashboard toggles are disabled in this mode.

## Manual mode (`settings/mode = "manual"`)

The dashboard writes booleans to `devices/light`, `fan`, `relay`, `buzzer`.
The ESP32 applies those values and does **not** run the automatic table.

Switching back to auto starts the table again on the next loop.

## Why relay is manual-only in auto

The relay is a spare low-voltage output. There is no extra sensor dedicated
to it. Inventing a second temperature rule would only duplicate the fan.

## Local fallback

If Firebase is down, automatic mode keeps using the last settings stored in
RAM (or the firmware defaults after a reset). Manual commands cannot arrive
until the cloud link works again.
