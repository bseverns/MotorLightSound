#include <StepperMotor.h>

void StepperMotor::moveToHome() {
  stepper.setCurrentPosition(0);
  stepper.setAcceleration(1000);
  stepper.setMaxSpeed(750); // reduce possible speed while homing
  stepper.setSpeed(1250);
  Serial.println("INIT, going home");
  homingInProgress = true;
  state = MOVING_TO_HOME;
  stepper.moveTo(40000);  // Move a fixed number of steps in the desired direction
}

void StepperMotor::reset() {
  state = MOVING_TO_SOFTHOME;
  homingInProgress = false;
  stepper.moveTo(0);
}

void StepperMotor::moveToPosition(long position) {
  state = MOVING_TO_POSITION;
  stepper.moveTo(-1 * position);
}

void StepperMotor::stop() {
  stepper.stop();
  state = STOPPED;
}

void StepperMotor::update() {
  bool currentSwitchState = digitalRead(homeSwitchPin);
  cpos = stepper.currentPosition();

  if (state == MOVING_TO_HOME) {
    if (!homingInProgress && currentSwitchState != previousSwitchState) {
      homingInProgress = true;
      state = BACKING_OFF_SWITCH;
      stepper.stop();     // Stop the motor
      stepper.setCurrentPosition(0);
      stepper.setMaxSpeed(2000);
      stepper.setSpeed(2000);
      stepper.setAcceleration(10000);
      Serial.println(stepper.currentPosition());
      Serial.println("home");
      stepper.move(-150);  // Bounce back a bit
    }
  } else if (state == BACKING_OFF_SWITCH) {
    if (!stepper.isRunning()) {
      state = AT_SOFT_HOME;
      Serial.println("resetting home");
      stepper.setCurrentPosition(0);  // Reset the current position as START_ZERO
    }
  } else if (state == MOVING_TO_SOFTHOME) {
    if (!stepper.isRunning()) {
      state = AT_SOFT_HOME;
    }
  } else if (state == AT_SOFT_HOME) {
    Serial.println("soft home");
    state = STOPPED;
    homingInProgress = false;
  } else if (state == MOVING_TO_POSITION) {
    homingInProgress = false;
    if (!stepper.isRunning()) {
      Serial.print("cpos: ");
      Serial.println(cpos);
      // Position reached
      state = STOPPED;
    }
  } else if (state == AT_END) {
    reset();
    // Handle AT_END state if needed
  }

  if (currentSwitchState != previousSwitchState) {
    if (currentSwitchState == LOW) {
      state = AT_END;
      stepper.stop();
      Serial.println("At End, must rehome");
    }
  }

  stepper.run();

  previousSwitchState = currentSwitchState;
}

MotorState StepperMotor::getState() {
  return state;
};

void StepperMotor::setSpeed(int speed) {
  if (speed == 1) {
    stepper.setMaxSpeed(1750);
    stepper.setSpeed(1750);
    Serial.println("slow");
  } else if (speed == 2) {
    Serial.println("med");
    stepper.setMaxSpeed(2000);
    stepper.setSpeed(2000);
  } else if (speed == 3) {
    Serial.println("fast");
    stepper.setMaxSpeed(2250);
    stepper.setSpeed(2250);
  } else {
    Serial.println("invalid speed");
  }
}

bool StepperMotor::isAtHome() {
  return state == AT_SOFT_HOME;
}