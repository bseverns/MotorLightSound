#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include "StepperControl.h"
#include "AudioManager.h"
#include "LEDManager.h"
#include "SerialManager.h"

class GameControl {
public:
    GameControl(StepperControl steppers[], int nSteppers, AudioManager& audioManager, LEDManager& ledManager, SerialManager& serialManager);
    void initialize();
    void startGame();
    void endGame(int laneId);
    void homeAllSteppers();
    void runSteppers();
    bool anyStepperMoving();
    void checkGameState();
    void resetGame();

private:
    StepperControl* steppers;
    int nSteppers;
    AudioManager& audioManager;
    LEDManager& ledManager;
    SerialManager& serialManager;
    bool isGameStarted;
    bool resettingGame;
    bool* isHomed;
    bool* reverseDirection;
    uint8_t winningLane;
};

#endif