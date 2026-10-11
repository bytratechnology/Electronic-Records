# Evidence Checklist

Code and diagrams show what I designed. This list is what proves I built
and tested it. An admissions reader or interviewer trusts a photo of a
messy breadboard and a table of real numbers far more than a polished
description.

Tick an item only when the file exists in the repository.

## For every project (01–14)

- [ ] `images/prototype.jpg` — the whole breadboard, in focus, wires visible
- [ ] One close-up of the main sensor or module
- [ ] Every row of the README test checklist changed from TODO to a real
      result (PASS, FAIL, or a number) with the date
- [ ] Failed tests left in, with a note on the cause and the fix
- [ ] The *Next step* and *Limitations* sections updated with what the real
      build taught me

## Screenshots (real, while the device is running)

| Project | File | Shows |
| ------- | ---- | ----- |
| 01 | `images/serial-plotter.png` | Temperature and humidity lines |
| 02 | `images/serial-plotter.png` | Reading crossing both thresholds |
| 05 | `images/serial-plotter.png` | Light level, thresholds, lamp state |
| 06 | `images/page.png` | Live page with the one-hour chart |
| 07 | `images/telegram.png` | Online, alarm, and cleared messages (hide the chat ID) |
| 08 | `images/monitor.png` | Monitor page during an alarm |
| 09 | `images/page.png` | Control page |
| 10 | `images/page.png` | Status and settings |
| 11 | `images/sheet.png` | Attendance tab with **invented names only** |
| 12 | `images/web-app.png` | One channel *pending*, one confirmed |
| 13 | `images/viewer.png` | Chart of at least 24 hours |
| 14 | `images/dashboard/…` | Dashboard in auto and in manual mode |

Before saving any screenshot, check that it shows no Wi-Fi password, bot
token, API key, e-mail address, or real name.

## Measurements

Fill these into [technical-report.md](technical-report.md), section 8.

- [ ] Sensor calibration values (baseline, light levels, dry / wet)
- [ ] Five timed runs for each delay in the results log
- [ ] Pump flow in mL per second
- [ ] A 24-hour run of the data logger, with the number of readings stored
- [ ] One deliberate Wi-Fi outage test for projects 11 and 13

## Videos

Short, unedited, one take. Speak in English if you can; it is also
evidence of your English.

- [ ] 60–90 s demo of each of my three strongest projects, following the
      *Demo script* in its README
- [ ] One 2-minute overview: who I am, what I built, one thing that went
      wrong and how I fixed it
- [ ] Uploaded as *unlisted* videos, with links added to the root README

## Lab notebook

- [ ] `docs/lab-notebook.md` or scanned pages of a paper notebook: dates,
      sketches, wrong ideas, measurements
- [ ] At least three entries in the *Challenges I actually faced* section
      of [student-portfolio.md](student-portfolio.md)

## Repository hygiene

- [ ] My real name in `README.md`, `LICENSE`, and `student-portfolio.md`
- [ ] `git log` shows work spread over weeks or months, with meaningful
      messages, not one commit
- [ ] No secrets anywhere in the history (`git log -p | grep -i password`)
- [ ] Repository is public and the link opens without logging in
- [ ] Links in the README all work on github.com

## People

- [ ] A teacher or mentor has seen at least one project working and could
      mention it in a recommendation letter
- [ ] I have presented the capstone to someone (class, club, fair) and
      noted the date and their questions
