#ifndef MAIN_H
#define MAIN_H

#include "GameControl.h"
#include "AudioManager.h"
#include "LEDManager.h"
#include "SerialManager.h"

extern const int N_STEPPERS;
extern StepperControl steppers[];
extern AudioManager audioManager;
extern LEDManager ledManager;
extern int laneIndPins[];
extern SerialManager serialManager;
extern GameControl gameControl;

void setup();
void loop();
void serialEvent1();
void serialEvent6();
void serialEvent7();
void serialEvent8();

#endif