#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include "StepperControl.h"
#include "AudioManager.h"
#include "LEDManager.h"
#include "SerialManager.h"

#include <Arduino.h>
#include <vector>

class GameControl {
public:
    static constexpr long TRACK_LENGTH_STEPS = 14000;
    static constexpr uint8_t START_BUTTON_PIN = 2;
    static constexpr unsigned long START_SEQUENCE_DELAY_MS = 2200;

    GameControl(StepperControl steppers[], int nSteppers, AudioManager& audioManager, LEDManager& ledManager, SerialManager& serialManager);

    void initialize();

    void startGame();

    void endGame(int laneId);

    void homeAllSteppers();

    void runSteppers();

    bool anyStepperMoving() const;

    void checkGameState();

    void resetGame();

private:
    struct StepperState {
        bool homed = false;
        bool reversing = false;
    };

    enum class GameState : uint8_t {
        Idle,
        Homing,
        Starting,
        Running,
        Resetting
    };

    StepperControl* steppers;
    int nSteppers;
    AudioManager& audioManager;
    LEDManager& ledManager;
    SerialManager& serialManager;
    GameState state;
    uint8_t winningLane;
    unsigned long startSequenceTimestamp;
    std::vector<StepperState> stepperStates;

    void transitionTo(GameState nextState);
    void updateStepperState(int index);
    void updateStateMachine();
};

#endif