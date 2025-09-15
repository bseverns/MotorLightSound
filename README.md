# MotorLightSound Teaching Lab

This repo is a playground for exploring how motion control, addressable LEDs, and layered game
logic collide on a Teensy 4.1. The code base was tuned to be a lesson plan in C++ for embedded
systems: every subsystem is isolated, the wiring is documented, and the main game loop reads like a
checklist. Use it to teach, hack, or just flex your synthwave bowling alley fantasy.

## TL;DR for the impatient instructor

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension or CLI both work).
2. Plug in a Teensy 4.1 that is wired up like the diagram below.
3. `pio run -t upload` from the project root to flash the firmware.
4. Open a serial monitor at 9600 baud to see the sensor chatter and command log.
5. Mash the start button on pin 2 and watch the lanes duel it out in glorious neon.

## Hardware vibe check

```
 ┌──────────────────────────────────────────────────────────────────────┐
 │ Teensy 4.1                                                           │
 │                                                                      │
 │  Pins 12/14/16/18 → Step pulses for NEMA steppers (lanes 1–4)        │
 │  Pins 11/13/15/17 → Direction for the same steppers                  │
 │  Pins 30/32/36/38 → Home sensors (active LOW)                        │
 │  Pin  23         → NeoPixel data for the 42‑LED marquee              │
 │  Pins  3/4/5/6   → Lane health indicator LEDs                        │
 │  Pin   2         → Start button (wired to ground for "pressed")      │
 │  SD Card        → `/startGame.wav`, `/rollerGame.wav`, `/ballHit.wav`│
 │  Serial1/6/7/8  → Lane microcontrollers reporting SENSOR_OK/ERROR   │
 └──────────────────────────────────────────────────────────────────────┘
```

Power the motors and LED strip according to their datasheets — nothing exotic is assumed except a
common ground with the Teensy.

## Software architecture at a glance

The firmware is intentionally carved into five managers so students can reason about one thing at a
time:

| Module | What it teaches | Highlights |
| ------ | ---------------- | ---------- |
| `StepperControl` | Wrapping third-party libraries with a clean API | Exposes home/drive/reset helpers around AccelStepper. |
| `LEDManager` | Talking to addressable LEDs | Hides the Adafruit NeoPixel details and centralizes color palettes. |
| `AudioManager` | Audio graph construction | Shows how to wire up `AudioConnection` objects once during init. |
| `SerialManager` | Robust serial parsing | Debounces lane health and stubs out a command channel. |
| `GameControl` | Cooperative state machines | Drives the whole show without blocking the loop. |

The `src/main.cpp` file then wires those managers together and keeps the Arduino `loop()` tight —
perfect for illustrating the "tiny slices of work" philosophy that embedded apps thrive on.

## Game state machine

`GameControl` models the gameplay as a handful of explicit states. This diagram mirrors the code and
is fantastic whiteboard fodder:

```
┌─────┐    start button     ┌─────────┐      homed +
│Idle │ ─────────────────▶ │ Homing  │ ────────────────┐
└─────┘                    └─────────┘               │
   ▲                              │                  │
   │                              │ start sound done │
   │                              ▼                  │
   │                        ┌──────────┐             │
   │                        │ Starting │             │
   │                        └──────────┘             │
   │                              │ start delay      │
   │                              ▼                  │
   │                        ┌──────────┐   win lane  │
   │                        │ Running  │ ───────────▶│
   │                        └──────────┘             ▼
   │                              │           ┌───────────┐
   │                              ▼           │ Resetting │
   └──────────────────────────────────────────┴───────────┘
```

Key teaching points:

* `startGame()` only flips the state when the system is idle, so students can reason about inputs
  without race conditions.
* Homing runs asynchronously — no `delay()` calls, just repeated checks until every motor hits its
  limit switch.
* The 2.2 second countdown before the chase lights and roller audio kick in is handled with a
  timestamp, demonstrating non-blocking delays.
* Reset completes when every stepper reports `isRunning() == false`, mirroring the homing logic and
  tying the loop together.

## Serial protocol sketch

Lane controllers are expected to stream newline-terminated messages. Anything that starts with
`SENSOR_OK` or `SENSOR_ERROR` toggles the corresponding indicator LED. If a payload includes a colon,
the trailing character is treated as a command (think `SENSOR_OK:R`). The default handler just logs
the command and nudges learners to hook it into `GameControl` for bonus credit.

## Suggested lessons & experiments

* **State machine drills** – Ask students to add a `Celebrating` state that twinkles the LEDs before
  homing. They can trace how to extend the enum, transition logic, and docs in one sweep.
* **Serial handshakes** – Have each lane microcontroller send `SENSOR_ERROR:R` and show how
  `handleCommand()` can bubble the reset request back to a higher-level system.
* **Audio swaps** – Replace `rollerGame.wav` with a student-built track to emphasize asset management
  on SD cards and AudioMemory sizing.
* **Cooperative multitasking** – Use the structure in `loop()` as the jumping off point for writing a
  new manager (scoreboard, attract mode, diagnostics, etc.).

## Tooling tips

* Format-on-save is intentionally off. Encourage learners to reason about whitespace and readability
  instead of relying on automation.
* PlatformIO unit tests can live under `test/`. They are empty right now so that you can scaffold
  your own exercises (mocking steppers, faking serial input, you name it).
* Mac users: `.DS_Store` files are ignored at the repo root so accidental Finder clicks stop polluting
  your diffs.

Have fun, be loud, and teach like your circuits are screaming along with you.

