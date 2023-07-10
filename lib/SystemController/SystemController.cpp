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

// SystemController.cpp
SystemController::SystemController(StepperMotor& motor1, StepperMotor& motor2, StepperMotor& motor3, StepperMotor& motor4, LightStrip* lightStrip, int buttonPin) 
    : motor1(motor1), 
      motor2(motor2), 
      motor3(motor3), 
      motor4(motor4), 
      lightStrip(lightStrip),
      buttonPin(buttonPin),
      state(IDLE) 
{
}

void SystemController::begin() {
    state = IDLE;
    lightStrip->begin();
}

void SystemController::update() {
    handleButtonPress();
    handleSerialCommand();

    switch (state) {
        case IDLE:
            // Do nothing
            break;
        case HOMING:
            motor1.update();
            motor2.update();
            motor3.update();
            motor4.update();
            if (motor1.isAtHome() && motor2.isAtHome() && motor3.isAtHome() && motor4.isAtHome()) {
                state = RUNNING;
            }
            break;
        case RUNNING:
            motor1.update();
            motor2.update();
            motor3.update();
            motor4.update();
            lightStrip->loop();
            break;
        case STOPPED:
            // Do nothing
            break;
    }
}

void SystemController::handleButtonPress() {
    if (digitalRead(buttonPin) == LOW) {
        start();
    }
}

void SystemController::handleSerialCommand() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');

        if (command.startsWith("m")) {
            int steps = command.substring(1).toInt();
            motor1.moveToPosition(random(steps));
            motor2.moveToPosition(random(steps));
            motor3.moveToPosition(random(steps));
            motor4.moveToPosition(random(steps));
        } else if (command.startsWith("s")) {
            int speed = command.substring(1).toInt();
            motor1.setSpeed(speed);
            motor2.setSpeed(speed);
            motor3.setSpeed(speed);
            motor4.setSpeed(speed);
        } else if (command.startsWith("x")) {
            stop();
        } else if (command.startsWith("h")) {
            start();
        }
    }
}

void SystemController::start() {
    //lightStrip->begin();
    motor1.moveToHome();
    motor2.moveToHome();
    motor3.moveToHome();
    motor4.moveToHome();
    state = HOMING;
}

void SystemController::stop() {
    motor1.stop();
    motor2.stop();
    motor3.stop();
    motor4.stop();
    state = STOPPED;
}