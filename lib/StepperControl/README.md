# StepperControl crib notes

`StepperControl` is a whisper-thin wrapper around the AccelStepper library. It exists so students can
treat each motor as a first-class object without wading through library docs mid-lesson. Highlights:

* `initialize()` sets sane defaults (800 steps/second max speed, 500 steps/sec² acceleration) and wires
  the home sensor as an `INPUT_PULLUP` because the switches are active LOW.
* `home()` fires the motor toward the switch. Pair it with `resetPosition()` and the lesson naturally
  drifts into conversations about coordinate systems.
* `move()` remains relative, matching AccelStepper's semantics. That keeps the door open for showing off
  `moveTo()` later if you want absolute positioning labs.
* `run()` and `isRunning()` expose the cooperative multitasking vibe that the whole repo leans on.

Want to go deeper? Encourage learners to add soft limits or per-lane speed tweaks by subclassing this
wrapper instead of mangling the game logic.

