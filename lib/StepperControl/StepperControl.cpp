#include "StepperControl.h"
#include <Arduino.h>

StepperControl::StepperControl(int stepPin, int dirPin, int homeSensorPin)
    : stepper(AccelStepper(1, stepPin, dirPin)), homeSensorPin(homeSensorPin), homed(false), reverseDirection(false) {}

void StepperControl::initialize() {
    pinMode(homeSensorPin, INPUT_PULLUP);
    stepper.setPinsInverted(false, true, false);
    stepper.setMaxSpeed(800.0);
    stepper.setAcceleration(500.0);
}

void StepperControl::home() {
    homed = false;
    reverseDirection = false;
    stepper.move(-14000); // Move towards home
}

void StepperControl::move(long steps) {
    stepper.move(steps);
}

void StepperControl::run() {
    stepper.run();
}

bool StepperControl::isRunning() {
    return stepper.isRunning();
}

bool StepperControl::isHomed() {
    return homed;
}

void StepperControl::setHomed(bool homed) {
    this->homed = homed;
}

void StepperControl::setReverseDirection(bool reverse) {
    this->reverseDirection = reverse;
}

int StepperControl::getHomeSensorPin() const {
    return homeSensorPin;
}

void StepperControl::stop() {
    stepper.stop();
}

void StepperControl::setSpeed(float speed) {
    stepper.setSpeed(speed);
}

void StepperControl::resetPosition(long position) {
    stepper.setCurrentPosition(position);
}

long StepperControl::currentPosition() const {
    return stepper.currentPosition();
}