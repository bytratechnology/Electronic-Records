# Student Portfolio Summary

A short page for a teacher, counsellor, or admissions officer who will not
open every source file.

Text in [brackets] and every TODO must be replaced with my own facts before
this page is sent to anyone.

---

## Who I am

**[Student Name]**, [grade / year] at [School], [City, Country].
Intended field of study: [Electrical Engineering / Computer Engineering /
Computer Science / …].

Contact: [email] · Repository: [link]

## The portfolio in one paragraph

Over [number] months I designed and programmed fourteen electronics
projects, starting with a temperature display on an Arduino and ending
with an ESP32 smart-home model that reads five sensors, controls four
outputs, and is operated from a web dashboard through a cloud database.
Each project is documented with its wiring, its logic, a test plan, and
its limitations.

## Why I did this

[Write 3–5 sentences in your own words. What made you curious about
electronics? Was there a specific moment or problem at home or at school?
Admissions readers remember a concrete story, not “I am passionate about
technology”.]

## What I can do now

| Skill | Evidence |
| ----- | -------- |
| Read a component datasheet and wire it safely | Voltage divider for a 5 V sensor on a 3.3 V board (07, 14) |
| Turn a noisy sensor into a reliable decision | Baseline, smoothing, hysteresis (02, 05) |
| Write embedded C++ that does several things at once | State machines and `millis()` timing (03, 04, 09) |
| Design a control loop with safety limits | Pump bursts, soak time, daily cap (10) |
| Connect a device to web services | HTTP, HTTPS, MQTT, NTP (06, 07, 08, 11) |
| Use a cloud database with authentication | Firebase rules, live sync (12, 13, 14) |
| Build a simple web interface | HTML, CSS, JavaScript (06, 09, 10, 12, 13) |
| Design for failure | Works offline, retries, queues (07, 11, 13) |
| Document and test my own work | README, test checklist, limitations in every folder |

## Three projects I would show first

**1. Smart Home Capstone (14).** The largest system: modular firmware,
automatic and manual modes that do not fight each other, and a dashboard.
It shows that I can integrate many parts into one design.

**2. Cloud Data Logging (13).** The device keeps readings in a queue when
the Wi-Fi is down and uploads them later with their original times. It
shows that I think about what happens when things go wrong.

**3. Web App Device Control (12).** The interface shows a relay as ON only
after the device confirms it. It shows that I care whether software tells
the truth about hardware.

[Change this list if other projects are more personal to you. Choose the
ones you can talk about for five minutes without notes.]

## The idea that changed how I work

Early projects worked on my desk and then failed in small, annoying ways: a
lamp that flickered at dusk, an alarm that chattered at its threshold. I
learned that the first version that “works” is usually only correct for
the easy case. From project 05 onward every README has a *Limitations*
section, and from project 07 onward the main function of each device works
without the network.

[Rewrite this in your own words, or replace it with the lesson that was
really the most important for you.]

## Real results

I only report what I measured on the real hardware.

| Item | Value |
| ---- | ----- |
| Projects built and tested on hardware | TODO: __ of 14 |
| Longest continuous run of the data logger (13) | TODO |
| Delay from alarm to phone notification (07) | TODO |
| Delay from web click to relay (12) | TODO |
| Hours spent (estimate) | TODO |
| Shown or presented to | TODO (class, club, science fair, …) |

## Challenges I actually faced

TODO: After building, write two or three real problems here: what I saw,
what I tried, what the cause was, and how I fixed it. A real bug with a
real fix is worth more than a list of features.

## What I want to study next

[2–3 sentences linking these projects to your intended major. For example:
which university course, lab, or research area continues what you started
here? Be specific to each university you apply to.]

---

## Ready-to-adapt text for applications

These are starting points. Check every claim against what I have truly
done, and replace the bracketed parts.

**Activity description (about 150 characters)**

> Designed and programmed 14 electronics/IoT projects (Arduino, ESP32, cloud);
> documented wiring, tests, and limits on GitHub.

**Short description (about 50 words)**

> I taught myself embedded programming by building fourteen projects of
> increasing difficulty, from a temperature display to a cloud-connected
> smart-home model. I wrote the firmware in C++, built the web interfaces,
> and published everything with test plans and known limitations at [link].

**Interview or essay prompts to prepare**

- Which project failed the most times before it worked, and why?
- What would you change if you rebuilt the capstone from the start?
- Where did you get stuck and need help, and how did you find it?
- What is one limitation you could not solve, and what would you need to
  learn to solve it?

## A note on help and sources

[State honestly what help you had: a course, a teacher or mentor, online
tutorials, library example code, AI tools. Universities value independent
work, and they value honesty about sources even more. Be ready to explain
any line of code in this repository.]
