# Challenges

Use this page as a lab notebook. Each block is a template until you fill it
with something that actually happened on your bench.

Do not copy these templates into a university application as if they were
finished stories. Replace the italic hints with your own Serial logs, wrong
pin numbers, and the fix you used.

---

## Challenge 1 — (title)

**Problem**  
*What did you see? Example: DHT always printed nan.*

**What I tried**  
*First idea, second idea.*

**What went wrong**  
*Why that idea failed, if you know.*

**How I debugged it**  
*Serial prints, swapping the sensor, measuring 3.3 V on VCC, etc.*

**Final solution**  
*The change that made it work (pull-up, DATA on GPIO 4, DHT type, …).*

**What I learned**  

---

## Challenge 2 — (title)

**Problem**  

**What I tried**  

**What went wrong**  

**How I debugged it**  

**Final solution**  

**What I learned**  

---

## Challenge 3 — (title)

**Problem**  

**What I tried**  

**What went wrong**  

**How I debugged it**  

**Final solution**  

**What I learned**  

---

## Prompts you are likely to hit

These are not claimed as completed work. They are reminders of common
failure modes for this exact parts list:

- ESP32 ADC pin 34/35 are input-only — do not reuse them as LED outputs.
- MQ-2 analog without a divider can over-voltage the pin.
- Relay modules are often active-LOW, so HIGH on boot can look “backwards”.
- PIR stay-high time is set by the module potentiometer *and* by `motion_hold_ms`.
- Firebase `begin()` succeeding is not the same as `Firebase.ready()`; wait for the token.
- Dashboard opened as `file://` may fail to load `firebase-config.js` the way you expect — use a local HTTP server.
