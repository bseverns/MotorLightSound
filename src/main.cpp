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
SystemController systemController(motor1, motor2, motor3, motor4, &lightStrip, BUTTON_PIN);

void setup() {
  delay(250);
  Serial.begin(115200);
  Serial.println("Setup started");
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // call begin to do the initialization of the system here
  systemController.begin();
  Serial.println("Setup completed");
}

void loop() {
    systemController.update();
}