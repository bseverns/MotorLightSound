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

#include <SystemController.h>
#include <LightStrip.h>
#include <StepperMotor.h>

int globalIndex = 1;
int cycles = 0;

SystemController::SystemController(StepperMotor motor1, StepperMotor motor2, StepperMotor motor3, StepperMotor motor4, LightStrip* lightStrip) 
    : motor1(motor1), 
      motor2(motor2), 
      motor3(motor3), 
      motor4(motor4), 
      lightStrip(lightStrip),
      buttonState(false), 
      state(IDLE) 
{
}

void SystemController::begin() {
  motor1.moveToHome();
  motor2.moveToHome();
  motor3.moveToHome();
  motor4.moveToHome();
  }

void SystemController::update() {
  // Update the system state
}

void SystemController::handleButtonPress() {
start();
}

void SystemController::start() {
  // Start the system
      lightStrip->begin();
      motor1.moveToHome();
      motor2.moveToHome();
      motor3.moveToHome();
      motor4.moveToHome();
}

void SystemController::stop() {
  // Stop the system
}