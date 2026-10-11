# Application Guide

How to turn this repository into a strong part of a university
application. This page is written for the student and for the teacher or
counsellor helping them. It is not meant to be sent to a university.

## What a portfolio can and cannot do

No document guarantees admission. Selective universities look at grades,
course rigour, test scores, essays, recommendations, and activities
together. What a technical portfolio does is make one claim believable:
*this person builds things, finishes them, and understands them.*

It only works if it is true. Admissions readers and interviewers see many
polished project pages. What they rarely see, and what they remember, is:

1. **Evidence** — real photos, real numbers, a commit history over time.
2. **Depth** — one design decision explained properly beats ten features listed.
3. **Honesty** — stated limits, failed tests, and credit for help received.
4. **Reflection** — what changed in how you think, not only what you made.
5. **Your own voice** — a specific reason you cared about this.

Everything below is about producing those five things.

## Step 1 — Make it yours (before anything else)

- [ ] Build every project you intend to mention. Do not list a project you
      have not built.
- [ ] Work through [evidence-checklist.md](evidence-checklist.md).
- [ ] Read every source file until you can explain any line without notes.
      If you cannot explain something, change it until you can, or remove it.
- [ ] Replace each sentence in the READMEs that does not match your real
      experience. The text is a starting point; where your build differed,
      your version is the correct one.
- [ ] Change at least one thing in each project because *you* wanted to: a
      different sensor, an extra feature, a fix for a listed limitation.
      Write down why.
- [ ] Fill in *A note on help and sources* in
      [student-portfolio.md](student-portfolio.md) truthfully (courses,
      mentors, tutorials, libraries, AI tools).

An interviewer who asks “why 10 kΩ?” or “what happens if the Wi-Fi drops
here?” is not trying to trap you. They are checking that the work is
yours. Section *Interview preparation* lists such questions.

## Step 2 — Choose a theme

Fourteen projects are too many to talk about. Pick the thread that is
really yours and lead with two or three projects that carry it.

| If what you care about is… | Lead with | The thread |
| -------------------------- | --------- | ---------- |
| Safety and reliability | 07, 08, 13 | “Systems that keep working when something fails” |
| Hardware and circuits | 05, 10, 14 | “From a sensor's physics to a stable decision” |
| Software and systems | 12, 13, 14 | “Making devices and interfaces agree on the truth” |
| Helping a specific community | 11, 10, 14 | “Tools for my school / family / farm” |
| Learning itself | 01 → 05 → 14 | “How fourteen small steps taught me to engineer” |

A theme tied to a real place or person in your life is stronger than any
of these. If your grandmother's kitchen is the reason for the gas alarm,
that is your theme.

## Step 3 — Where the portfolio goes in an application

Rules and limits change every year. **Check each university's current
instructions**; the notes below describe common patterns.

### Activities list (for example, the Common Application)

One entry, with very little space. Lead with verbs and numbers.

> **Position / role:** Independent Electronics & IoT Developer
>
> **Description (about 150 characters):** Designed, built, tested 14
> devices (Arduino, ESP32, cloud); wrote firmware + web apps; published
> code, test results, limits on GitHub.

State honest hours per week and weeks per year. Inflated numbers are easy
to spot next to a commit history.

### Additional information section

Use it for facts that fit nowhere else: the repository link, two or three
sentences on the capstone, and one measured result. Do not paste an essay
here. Keep it short; word limits for this section have been reduced in
recent years.

### Maker or technical portfolio

Some universities (for example MIT) invite a separate maker portfolio:
typically a short video of about two minutes, a few images, and a short
description of what you made, why, and what help you had. Use:

- the 2-minute overview video from the evidence checklist,
- the capstone wiring diagram, one real photo, one screenshot,
- your own answer to “what did you do yourself?”

### Personal essay or personal statement

The portfolio is evidence for the essay, not the essay. See Step 4.

### Recommendation letters

Show a teacher a working project in person and send them the one-page
[student-portfolio.md](student-portfolio.md). A letter that says “I watched
her debug this for three weeks” is worth more than anything you write
about yourself.

### Interviews

Bring one small working project if it is allowed, or have a 60-second
video ready on your phone. Use the *Demo script* in that project's README.

### Scholarships

Scholarship committees often ask about impact and initiative. Use
measured results and any real users (a class using the attendance system,
a family member using the gas alert).

## Step 4 — Writing about a project

### What to avoid

| Weak | Why | Stronger |
| ---- | --- | -------- |
| “I am passionate about technology.” | Anyone can write it | A specific moment that started it |
| “I built a smart home system with 5 sensors.” | A list of features | One problem inside it and how you solved it |
| “The project was a success.” | No evidence | A number you measured, or a test that failed first |
| “I learned a lot about teamwork and perseverance.” | Generic | The exact idea you understand now and did not before |
| Technical terms piled up | Reads like a manual | One term, explained in a sentence |

### A structure that works

1. **A concrete moment.** Where were you, what did you see go wrong?
2. **What you expected** and why that seemed reasonable.
3. **What actually happened.** The surprise.
4. **How you found the cause.** What you tried that did *not* work.
5. **The idea underneath.** The general principle you took away.
6. **Where it led.** The next project, a change in how you work, what you
   want to study.

### Story seeds in this portfolio

Each of these is a real engineering idea in the code. They become *your*
stories only if they happened to you. Use the ones that did, and tell them
the way they happened.

| Project | The moment | The idea underneath |
| ------- | ---------- | ------------------- |
| 05 | A lamp that flickered at dusk with a one-line program | “Works on my desk” is not “works”; hysteresis |
| 02 → 07 | Moving a 5 V sensor to a 3.3 V board | Reading a datasheet before connecting a wire |
| 07 | Deciding what happens when the Wi-Fi is down | Safety functions must not depend on the network |
| 10 | A pump that would have flooded the pot | Slow systems need patient controllers; plan for sensor failure |
| 11 | Realising the sheet contains classmates' names | Engineers are responsible for the data they collect |
| 12 | A button that turned green although nothing happened | An interface must tell the truth about the hardware |
| 13 | A chart with a hole in it after the router restarted | The time of a measurement belongs to the measurement |
| 14 | Auto mode and manual mode fighting over one LED | Large systems need clear ownership of each decision |

### Paragraph pattern for a short answer

> When I first [did X], I expected [Y]. Instead, [Z happened]. I found that
> [cause] by [how you investigated]. I fixed it by [change], and measured
> [result]. Since then I [habit or principle], which is why [link to what
> you want to study].

Write it, then delete every sentence that could appear in someone else's
application.

## Step 5 — Interview preparation

Prepare spoken answers, not memorised scripts. For each question, the
place to find the technical background is in brackets.

### About the work

1. Walk me through one project from sensor to output. [any README, *How it works*]
2. Why did you use two thresholds instead of one? [02, 05]
3. Why does the MQ-2 need a voltage divider on the ESP32 but not on the
   Arduino? [07, *Calculations*]
4. What does your gas alarm do if the Wi-Fi is down? [07; technical report §5]
5. What is the difference between your HTTP projects and your MQTT
   project? [08, comparison table]
6. Why do you store both “desired” and “reported” state? [12]
7. If your data logger loses Wi-Fi for an hour, what is in the database
   afterwards? [13]
8. How do you know the pump cannot flood the room? [10, *Failure modes*]
9. What is a state machine and why did you use one? [03, 04]
10. What is insecure about your RFID system? [11, *Security and privacy review*]

### About you

11. Which project failed the most times before it worked?
12. What is one thing in the repository you are not happy with?
13. What did you have help with, and what did you do alone?
14. What would you build next with one more month? With a budget?
15. What did this teach you that school did not?
16. Why this university and this course, given these projects?

For 11–16 there is no reference answer. Write yours in
[student-portfolio.md](student-portfolio.md).

### Good answer habits

- Start with the short answer, then give one detail.
- Say “I don't know, but I would find out by…” when that is the truth.
- Mention a limitation before the interviewer finds it.
- Use a number you measured.

## Step 6 — Timeline

A realistic schedule if applications are due in about six months. Adjust
the months to your own deadlines.

| When | Do |
| ---- | -- |
| Months 1–2 | Build Level 1 (01–05). Photos and test tables for each. Start the lab notebook. Commit weekly. |
| Months 2–4 | Build Level 2 (06–13). Record the measurements in the technical report. |
| Month 4 | Build the capstone (14). Run the 24-hour logger test. |
| Month 5 | Add your own extension to two or three projects. Record the videos. Show a teacher. |
| Month 5 | Draft essays from your notebook, not from the READMEs. Get feedback. |
| Month 6 | Final check with the evidence checklist. Make the repository public. Submit. |

Starting later? Build fewer projects properly rather than all fourteen
thinly. Five built, tested, and understood projects are a stronger
application than fourteen folders of untested code.

## Step 7 — Final check before sending a link

- [ ] Open the repository in a private browser window. Does it load?
- [ ] Does the first screen of the README show your name, one real photo,
      and what this is, in ten seconds?
- [ ] No TODO left in any project you mention in the application
- [ ] No secrets, no real names of other people
- [ ] Every number you quote in the application appears in the repository
- [ ] You can explain every file

## Academic honesty

Presenting work you did not do, or results you did not measure, as your
own is misrepresentation. Universities can and do withdraw offers for it,
sometimes years later. It is also unnecessary: an honest account of a
smaller project, with real evidence and clear thinking, is what this guide
is designed to help you produce.
