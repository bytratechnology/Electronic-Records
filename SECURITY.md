# Security and Safety

These are student prototypes, not products. They still follow a few rules
so that passwords never end up on GitHub and nobody gets hurt on the bench.

## Secrets that must never be committed

- Wi-Fi name and password
- Telegram bot token and chat ID (project 07)
- MQTT broker username and password (project 08)
- Google Apps Script URL and shared secret (project 11)
- Firebase user email and password (projects 12, 13, 14)
- Real names, student IDs, phone numbers, or home address

## Where the real values live

| File (gitignored) | Created from | Projects |
| ----------------- | ------------ | -------- |
| `firmware/include/config.h` | `firmware/config.example.h` | 06–13 |
| `monitor/config.js` | `monitor/config.example.js` | 08 |
| `web-app/firebase-config.js`, `viewer/firebase-config.js` | `firebase-config.example.js` | 12, 13 |
| Capstone credentials | see [14-smart-home-capstone/SECURITY.md](14-smart-home-capstone/SECURITY.md) | 14 |

After filling them in, run `git status` and confirm none of these files
appear as new files.

In projects 12 and 13 the web page asks for the email and password in a
sign-in form, so no password is stored in any JavaScript file.

## Cloud rules

- Firebase Realtime Database rules must require a signed-in user
  (`auth != null`). Never leave a database public after a demo.
- Use a separate Firebase user for the ESP32, so it can be disabled
  without locking yourself out.
- A public MQTT test broker is readable by anyone. Do not send personal
  data through it.

## Personal data

Project 11 handles attendance records. Screenshots and example data in
this repository use invented names only.

## Electrical safety

- All projects run on USB 5 V or 3.3 V. **No project switches mains
  electricity (220 V / 110 V).** Relays switch low-voltage DC loads only.
- ESP32 pins accept 3.3 V at most. 5 V sensor outputs go through a voltage
  divider (see projects 07 and 14).
- Motors and pumps use their own supply with a common ground (project 10).
- Flame and gas tests are done briefly, in a ventilated room, on a
  non-flammable surface, with an adult or teacher present.
- These devices are learning models. They are not certified fire, gas, or
  security equipment and must not be relied on for safety.

## If a secret leaks

1. Assume it is compromised and replace it (new Wi-Fi password, revoke the
   bot token with @BotFather, new Apps Script secret and deployment, disable
   the Firebase user).
2. Remove it from the git history before making the repository public.
