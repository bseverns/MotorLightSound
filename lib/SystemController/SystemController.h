# ifndef SYSTEMCONTROLLER_H
# define SYSTEMCONTROLLER_H

#include <StepperMotor.h>
#include <LightStrip.h>

// SystemController.h
class SystemController {
private:
    StepperMotor& motor1;
    StepperMotor& motor2;
    StepperMotor& motor3;
    StepperMotor& motor4;
    LightStrip* lightStrip;
    int buttonPin;

    enum SystemState {
        IDLE,
        HOMING,
        RUNNING,
        STOPPED
    } state;

public:
    SystemController(StepperMotor& motor1, StepperMotor& motor2, StepperMotor& motor3, StepperMotor& motor4, LightStrip* lightStrip, int buttonPin);

    void begin();
    void update();
    void handleButtonPress();
    void handleSerialCommand();
    void start();
    void stop();
};

#endif // SYSTEMCONTROLLER_H