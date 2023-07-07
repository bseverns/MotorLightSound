# ifndef SYSTEMCONTROLLER_H
# define SYSTEMCONTROLLER_H

#include <StepperMotor.h>
#include <LightStrip.h>

class SystemController {
private:
    StepperMotor motor1;
    StepperMotor motor2;
    StepperMotor motor3;
    StepperMotor motor4;
    LightStrip* lightStrip;  // Use a pointer to LightStrip

    // State of the button
    bool buttonState;

    // Current state of the system
    enum SystemState {
        IDLE,
        HOMING,
        RUNNING,
        STOPPED
    } state;

public:
   // Constructor
    SystemController(StepperMotor motor1, StepperMotor motor2, StepperMotor motor3, StepperMotor motor4, LightStrip* lightStrip);


    // Initialize the system
    void begin();

    // Update the system state
    void update();

    // Handle button press
    void handleButtonPress();

    // Start the system
    void start();

    // Stop the system
    void stop();
};

#endif // SYSTEMCONTROLLER_H