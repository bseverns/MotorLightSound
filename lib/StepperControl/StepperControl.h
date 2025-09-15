#ifndef STEPPER_CONTROL_H
#define STEPPER_CONTROL_H

#include <AccelStepper.h>

class StepperControl {
public:
    StepperControl(int stepPin, int dirPin, int homeSensorPin);
    void initialize();
    void home();
    void move(long steps);
    void run();
    bool isRunning();
    bool isHomed();
    void setHomed(bool homed);
    void setReverseDirection(bool reverse);
    int getHomeSensorPin() const;
    void stop();
    void setSpeed(float speed);
    void resetPosition(long position = 0);
    long currentPosition() const;

private:
    AccelStepper stepper;
    int homeSensorPin;
    bool homed;
    bool reverseDirection;
};

#endif