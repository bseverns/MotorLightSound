# AudioManager notes

`AudioManager` shows off how the Teensy audio library lets you build a signal chain once and then reuse
it. The class is intentionally tiny so students can map code to the graphical patch editor docs.

* `initialize()` allocates twelve audio blocks (`AudioMemory(12)`) and wires three WAV players into three
  mixers feeding the shared I²S output. Each patch cord allocation is a talking point about dynamic
  memory on embedded targets — feel free to replace it with a static array when demonstrating RAII.
* `playStartGameSound()`, `playRollerGameSound()`, and `playBallHitSound()` highlight how multiple WAV
  players can coexist without stepping on each other.
* `playBallHitSound()` guards the effect so it only retriggers when the channel is free. That's a sneaky
  chance to talk about idempotence and resource contention.

Pair this module with an SD card scavenger hunt where students swap out the WAV files and measure memory
usage.

