#include "GameControl.h"

#include <cstdlib>

GameControl::GameControl(
    StepperControl steppers[],
    int nSteppers,
    AudioManager& audioManager,
    LEDManager& ledManager,
    SerialManager& serialManager)
    : steppers(steppers),
      nSteppers(nSteppers),
      audioManager(audioManager),
      ledManager(ledManager),
      serialManager(serialManager),
      state(GameState::Idle),
      winningLane(0),
      startSequenceTimestamp(0),
      stepperStates(nSteppers) {}

void GameControl::initialize() {
    pinMode(START_BUTTON_PIN, INPUT_PULLUP);

    for (int i = 0; i < nSteppers; i++) {
        steppers[i].initialize();
    }

    audioManager.initialize();
    ledManager.initialize();
    serialManager.initialize();

    resetGame();
}

void GameControl::startGame() {
    if (state != GameState::Idle) {
        return;
    }

    ledManager.clear();
    ledManager.show();

    homeAllSteppers();
    transitionTo(GameState::Homing);
}

void GameControl::endGame(int laneId) {
    ledManager.clear();

    if (laneId >= 0 && laneId < nSteppers) {
        winningLane = static_cast<uint8_t>(laneId);
        ledManager.fill(ledManager.getLaneColor(winningLane));
    }

    ledManager.show();
    homeAllSteppers();
    transitionTo(GameState::Resetting);
}

void GameControl::homeAllSteppers() {
    for (int i = 0; i < nSteppers; i++) {
        stepperStates[i] = StepperState{};
        steppers[i].home();
    }
}

void GameControl::runSteppers() {
    for (int i = 0; i < nSteppers; i++) {
        updateStepperState(i);
        steppers[i].run();
    }
}

bool GameControl::anyStepperMoving() const {
    for (int i = 0; i < nSteppers; i++) {
        if (steppers[i].isRunning()) {
            return true;
        }
    }

    return false;
}

void GameControl::checkGameState() {
    if (state == GameState::Idle && digitalRead(START_BUTTON_PIN) == LOW) {
        startGame();
    }

    updateStateMachine();
}

void GameControl::resetGame() {
    winningLane = 0;

    for (auto& stepperState : stepperStates) {
        stepperState = StepperState{};
    }

    ledManager.clear();
    ledManager.show();

    transitionTo(GameState::Idle);
}

void GameControl::transitionTo(GameState nextState) {
    state = nextState;
}

void GameControl::updateStepperState(int index) {
    StepperState& currentState = stepperStates[index];
    const int homeSensorValue = digitalRead(steppers[index].getHomeSensorPin());

    if ((state == GameState::Homing || state == GameState::Resetting) && !currentState.homed) {
        if (homeSensorValue == LOW) {
            steppers[index].stop();
            steppers[index].setSpeed(0);
            steppers[index].resetPosition();
            currentState.homed = true;
            currentState.reversing = false;
        }
        return;
    }

    if (state == GameState::Running && currentState.homed) {
        if (!steppers[index].isRunning()) {
            const long target = currentState.reversing ? -TRACK_LENGTH_STEPS : TRACK_LENGTH_STEPS;
            steppers[index].move(target);
            currentState.reversing = !currentState.reversing;
        }
    }
}

void GameControl::updateStateMachine() {
    switch (state) {
        case GameState::Idle:
            break;
        case GameState::Homing:
            if (!anyStepperMoving()) {
                audioManager.playStartGameSound();
                startSequenceTimestamp = millis();
                transitionTo(GameState::Starting);
            }
            break;
        case GameState::Starting:
            if (millis() - startSequenceTimestamp >= START_SEQUENCE_DELAY_MS) {
                audioManager.playRollerGameSound();
                ledManager.startChase(LEDManager::getColor(255, 0, 0));
                transitionTo(GameState::Running);
            }
            break;
        case GameState::Running:
            for (int i = 0; i < nSteppers; i++) {
                if (labs(steppers[i].currentPosition()) >= TRACK_LENGTH_STEPS) {
                    endGame(i);
                    break;
                }
            }
            break;
        case GameState::Resetting:
            if (!anyStepperMoving()) {
                resetGame();
            }
            break;
    }
}
