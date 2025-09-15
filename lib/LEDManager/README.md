# LEDManager quick tour

This wrapper keeps the Adafruit NeoPixel API at arm's length so students can focus on animation logic.
Use it to discuss:

* **Color palettes** – `laneColors` holds four intentionally loud hues. The helper `getLaneColor()` makes
  it trivial to map lane IDs to colors during a lesson on game feedback.
* **Chase patterns** – `startChase()` is blocking on purpose. It's a great conversation starter about why
  long delays can freeze a game loop and how you might redesign it to be non-blocking homework.
* **Utility helpers** – `clear()`, `fill()`, and the static `getColor()` helper demonstrate how to expose
  just enough of the underlying library for students to experiment without footguns.

The strip is constructed once with `(ledCount, ledPin, NEO_GRB + NEO_KHZ800)` so learners can port the
lesson to other hardware by changing one line.

