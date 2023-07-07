#include "Arduino.h"
#include <AccelStepper.h>
#include <LightStrip.h>
#include <StepperMotor.h>
#include <SystemController.h>

// Pin definitions
// lane 4
#define DIR_PIN4 11
#define STEP_PIN4 12
#define SWITCH_PIN4 38

// lane 3
#define DIR_PIN3 15
#define STEP_PIN3 16
#define SWITCH_PIN3 36

// lane 2
#define DIR_PIN2 13
#define STEP_PIN2 14
#define SWITCH_PIN2 32

// lane 1
#define DIR_PIN1 17
#define STEP_PIN1 18
#define SWITCH_PIN1 30

#define BUTTON_PIN 2

StepperMotor motor1(STEP_PIN1, DIR_PIN1, SWITCH_PIN1);
StepperMotor motor2(STEP_PIN2, DIR_PIN2, SWITCH_PIN2);
StepperMotor motor3(STEP_PIN3, DIR_PIN3, SWITCH_PIN3);
StepperMotor motor4(STEP_PIN4, DIR_PIN4, SWITCH_PIN4);

LightStrip lightStrip;
SystemController systemController(motor1, motor2, motor3, motor4, &lightStrip);

void setup() {
  delay(250);
  Serial.begin(115200);
  lightStrip.begin();
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // call begin to do the initialization of the system here
  systemController.begin();
  }

void loop() {

  // Check for a button press
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Reset the light strip cycles
    lightStrip.resetCycles();

    // Re-initiate homing for stepper motors
    motor1.moveToHome();
    motor2.moveToHome();
    motor3.moveToHome();
    motor4.moveToHome();
  }
  
  // Call the update functions for each stepper
  motor1.update();
  motor2.update();
  motor3.update();
  motor4.update();

  // Update the light strip
  lightStrip.loop();

  // Add the serial controls back
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    Serial.println(command);

    if (command.startsWith("m")) {
      int steps = command.substring(1).toInt();
      Serial.print("-- steps: ");
      Serial.println(steps);
      motor1.moveToPosition(random(steps));
      motor2.moveToPosition(random(steps));
      motor3.moveToPosition(random(steps));
      motor4.moveToPosition(random(steps));
      // motor.move(steps);
    } else if (command.startsWith("s")) {
      int speed = command.substring(1).toInt();
      Serial.print("-- speed: ");
      Serial.println(speed);
      motor1.setSpeed(speed);
      motor2.setSpeed(speed);
      motor3.setSpeed(speed);
      motor4.setSpeed(speed);
    } else if (command.startsWith("x")) {
      Serial.println("--stop");
      motor1.stop();
      motor2.stop();
      motor3.stop();
      motor4.stop();
    } else if (command.startsWith("h")) {
      Serial.println("--home");
      motor1.moveToHome();
      motor2.moveToHome();
      motor3.moveToHome();
      motor4.moveToHome();
    } else {
      Serial.print("err: ");
      Serial.println(command);
    }
  }
}