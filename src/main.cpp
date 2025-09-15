#include "main.h"

const int N_STEPPERS = 4;
StepperControl steppers[N_STEPPERS] = {
    StepperControl(12, 11, 30), // Lane 1
    StepperControl(14, 13, 32), // Lane 2
    StepperControl(16, 15, 36), // Lane 3
    StepperControl(18, 17, 38)  // Lane 4
};

AudioManager audioManager;
LEDManager ledManager(23, 42);
int laneIndPins[4] = { 3, 4, 5, 6 };
SerialManager serialManager(4, laneIndPins);
GameControl gameControl(steppers, N_STEPPERS, audioManager, ledManager, serialManager);

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial6.begin(9600);
    Serial7.begin(9600);
    Serial8.begin(9600);

    // Kick the managers into life so the lesson starts from a known-good state.
    gameControl.initialize();
}

void loop() {
    // The order here illustrates the cooperative-multitasking pattern used throughout
    // the project. Each call does a tiny slice of work before we loop back around.
    gameControl.checkGameState();
    gameControl.runSteppers();
    serialEvent1();
    serialEvent6();
    serialEvent7();
    serialEvent8();
}

void serialEvent1() {
    serialManager.processSerialData(Serial1, 0);
}

void serialEvent6() {
    serialManager.processSerialData(Serial6, 1);
}

void serialEvent7() {
    serialManager.processSerialData(Serial7, 2);
}

void serialEvent8() {
    serialManager.processSerialData(Serial8, 3);
}