# Future development

Everything below is **future work**. It is not implemented in this repository
and should not be described as a current feature in an application essay.

## Mobile app

A small phone client that uses the same `smart_home` tree would make demos
easier than a laptop browser. It would still need the same auth and rules.

## Energy monitoring

A current sensor (for example INA219) on the 5 V rail could log fan/LED
draw. That is still a model-scale measurement, not household kWh.

## Smart scheduling

Time-of-day rules (fan only in the afternoon) need NTP on the ESP32 or
schedules stored in Firebase. Not in the current loop.

## Weather API

Outdoor temperature from a public API could bias the fan threshold. This
needs another network client and a clear display of “indoor vs outdoor.”

## Voice control

Voice would be another command source, like the dashboard. It should still
respect Manual vs Auto. Not started.

## Machine learning

Occupancy or gas-pattern models would need labelled logs from
`smart_home/history`. Collecting that dataset is a project of its own.
There is **no** ML model in the firmware today.

## Occupancy prediction

PIR only sees motion in a cone. Future work could combine light, time of
day, and PIR with a hold timer that is learned rather than fixed
`motion_hold_ms`.

## Energy optimization

Turning the fan off with hysteresis (separate on/off thresholds) would
reduce chatter near 30 °C. A small dead-band is a realistic next firmware
change; full “energy optimization” is a larger study.

## If I continue this project

The first increment I would actually build is hysteresis on the fan and a
simple max length for `history`, because both stay within the current
hardware.
