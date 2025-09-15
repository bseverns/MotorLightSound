# SerialManager cheat sheet

This class is the interpreter that sits between the Teensy and whatever microcontrollers live under
each bowling lane. It focuses on two lessons:

* **Signal health indicators** – The first chunk of a message determines whether the lane is happy.
  `SENSOR_OK` flips the indicator LED on, `SENSOR_ERROR` flips it off. The flashing sequence in
  `initialize()` gives students a quick power-on self test.
* **Command channel** – Messages can optionally contain a trailing `:X`. The character after the colon
  is treated as a command and handed to `handleCommand()`, which currently just logs the request with a
  little attitude. That makes it painfully obvious where to glue in higher-level hooks (like nudging
  `GameControl` to reset a lane) during a lab exercise.

Because `processSerialData()` uses `readStringUntil('\n')`, you can replay canned serial logs from a
file or a test harness without needing custom parsing code. Set the stream's timeout, feed it a string
per line, and watch the LEDs react.

