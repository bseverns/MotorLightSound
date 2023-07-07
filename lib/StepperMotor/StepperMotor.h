# ifndef STEPPERMOTOR_H
# define STEPPERMOTOR_H

#include "Arduino.h"
#include <AccelStepper.h>

// What is the motors doing?
enum MotorState {
  INIT,
  MOVING_TO_HOME,
  MOVING_TO_SOFTHOME,
  BACKING_OFF_SWITCH,
  AT_SOFT_HOME,
  AT_END,
  MOVING_TO_POSITION,
  STOPPED
};

class StepperMotor {
private:
  AccelStepper stepper;
  int homeSwitchPin;
  MotorState state;
  bool previousSwitchState;
  long cpos = 0;
  bool homingInProgress = false;

public:
  StepperMotor(int step, int dir, int homeSwitch)
    : stepper(AccelStepper::DRIVER, step, dir),
      homeSwitchPin(homeSwitch),
      state(INIT),
      previousSwitchState(true)
  {
    pinMode(homeSwitchPin, INPUT_PULLUP);
    stepper.setMaxSpeed(250);
    stepper.setAcceleration(500000);
    stepper.setSpeed(250);
    stepper.setCurrentPosition(0);
  }

  void moveToHome();
  void reset();
  void moveToPosition(long position);
  void stop();
  void update();
  void move(int steps);
  MotorState getState();
  void setSpeed(int speed);
  bool isAtHome(); // Declaration of isAtHome() function
};

# endif // STEPPERMOTOR_H