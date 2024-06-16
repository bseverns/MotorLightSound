#include "main.h"

const int N_STEPPERS = 4;
StepperControl steppers[N_STEPPERS] = {
    StepperControl(12, 11, 30),
    StepperControl(14, 13, 32),
    StepperControl(16, 15, 36),
    StepperControl(18, 17, 38)
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

    gameControl.initialize();
}

void loop() {
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