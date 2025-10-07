# Lane-side control brain

This sketch is the scrappy microcontroller that lives under each bowling lane. It wrangles six
VL53L0X time-of-flight sensors, keeps a red beacon honest, and screams back to the Teensy mothership
whenever something rolls through the danger zone.

## Wiring crash course

* **Light pin** – Pin 13 is the lane indicator. It's your "yo, a ball just flew by" siren.
* **Sensor enable (XSHUT) pins** – Pins 4, 5, 6, 7, 10, and 11 gate power to each VL53L0X so we can set
  unique I2C addresses without them fighting on the bus.
* **Serial1** – Runs at 9600 baud. Hook it up to the RS-485/serial link that feeds back to the
  Teensy/SerialManager combo in this repo.

Power the sensors from 5V with a common ground. The VL53L0X boards handle level shifting on their own.

## What the firmware is doing

1. Boots every VL53L0X one at a time, slaps a unique address on it, and starts continuous distance reads.
2. Watches the sensors in three groups (front, middle, late). Each group maps to the `F`, `M`, and `L`
   characters that the Teensy expects.
3. Fires the lane beacon, ships the group character over Serial1, and waits for an `A` (acknowledge) or
   `R` (report status) reply.
4. Accepts a handful of commands from upstream:
   * `H` – Hold the beacon on.
   * `T` – Blink the beacon in a dramatic "the machine is thinking" pattern.
   * `O` – Force the light off.
   * `D` – Placeholder for future data dumps.
   * `R` – Dump sensor health back over USB for quick diagnostics.

## Building it without tears

* Install the [Pololu VL53L0X library](https://github.com/pololu/vl53l0x-arduino) (PlatformIO will pull
  it automatically thanks to `platformio.ini`).
* Flash the sketch onto whatever small micro you have glued to the lane rig (we used a stack of Pro
  Micros because they're cheap and hardy).
* Crack open a serial terminal at 9600 baud. You should see six "initialized successfully" messages
  followed by the sensor streams.

When it all works, the Teensy shows "SENSOR_OK" for that lane and the light pulses like the machine's
breathing. When it doesn't, the beacon goes into angry blink mode so you can slam the power switch and
start over. Punk rock diagnostics.
