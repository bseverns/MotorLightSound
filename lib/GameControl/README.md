# GameControl module

`GameControl` is the conductor for the whole arcade lane. It wraps four steppers, the LED marquee,
the SD-powered audio rig, and the serial status lines into a single state machine. The new
implementation is intentionally explicit so you can hand it to a classroom and say "trace the
states." Here's how it works:

1. `startGame()` only does work if the system is idle. That makes the start button debouncing lesson
   easy — you can literally watch the guard clause stop accidental re-triggers.
2. `homeAllSteppers()` resets the per-stepper bookkeeping struct and fires each motor toward its limit
   switch. The `StepperControl::resetPosition()` helper lands the encoder at zero as soon as the home
   sensor trips.
3. `updateStateMachine()` handles the transitions without blocking. Audio playback and LED chases still
   happen in the background while the rest of the managers keep breathing.
4. Once the running state is active, `updateStepperState()` waits for a motor to finish its current move
   before scheduling the next leg of the race. It's a clean hook for teaching cooperative motion
   control.
5. Winning a lane immediately paints it in that lane's color, then the reset state handles the slow
   drive back to home before dropping you into `Idle` again.

Want a project idea? Ask students to add a `Celebrating` state between `Running` and `Resetting` that
plays the `ballHit.wav` clip and strobes the LEDs. They'll have to reason through the transition helper
and the per-stepper state struct, and they'll leave understanding the structure better than when they
started.

