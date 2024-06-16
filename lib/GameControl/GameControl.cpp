#include "GameControl.h"

GameControl::GameControl(StepperControl steppers[], int nSteppers, AudioManager& audioManager, LEDManager& ledManager, SerialManager& serialManager)
    : steppers(steppers), nSteppers(nSteppers), audioManager(audioManager), ledManager(ledManager), serialManager(serialManager),
      isGameStarted(false), resettingGame(false) {
    isHomed = new bool[nSteppers];
    reverseDirection = new bool[nSteppers];
    for (int i = 0; i < nSteppers; i++) {
        isHomed[i] = false;
        reverseDirection[i] = false;
    }
}

void GameControl::initialize() {
    for (int i = 0; i < nSteppers; i++) {
        steppers[i].initialize();
    }
    audioManager.initialize();
    ledManager.initialize();
    serialManager.initialize();
}

void GameControl::startGame() {
    homeAllSteppers();
    while (anyStepperMoving()) {
        delay(10);
    }
    audioManager.playStartGameSound();
    delay(2200);
    audioManager.playRollerGameSound();
    isGameStarted = true;
    ledManager.startChase(LEDManager::getColor(255, 0, 0)); // Red color chase
}

void GameControl::endGame(int laneId) {
    ledManager.clear();
    if (laneId >= 0 && laneId < nSteppers) {
        winningLane = laneId;
        ledManager.fill(ledManager.getLaneColor(winningLane));
    }
    ledManager.show();
    homeAllSteppers();
    resettingGame = true;
}

void GameControl::homeAllSteppers() {
    for (int i = 0; i < nSteppers; i++) {
        isHomed[i] = false;
        reverseDirection[i] = false;
        steppers[i].home();
    }
}

bool GameControl::anyStepperMoving() {
    for (int i = 0; i < nSteppers; i++) {
        if (steppers[i].isRunning()) {
            return true;
        }
    }
    return false;
}

void GameControl::runSteppers() {
    for (int i = 0; i < nSteppers; i++) {
        if (!isHomed[i]) {
            if (digitalRead(steppers[i].getHomeSensorPin()) == LOW) {
                steppers[i].stop();
                steppers[i].setSpeed(0);
                reverseDirection[i] = true;
                isHomed[i] = true;
            }
        } else {
            if (digitalRead(steppers[i].getHomeSensorPin()) == LOW && !reverseDirection[i]) {
                steppers[i].move(14000); // Move in forward direction
                reverseDirection[i] = true;
            } else if (digitalRead(steppers[i].getHomeSensorPin()) == HIGH && reverseDirection[i]) {
                steppers[i].move(-14000); // Move in reverse direction
                reverseDirection[i] = false;
                isHomed[i] = false;
            }
        }
        steppers[i].run();
    }
}

void GameControl::checkGameState() {
    if (isGameStarted && digitalRead(2) == LOW) {
        startGame();
    }

    for (int i = 0; i < nSteppers; i++) {
        if (abs(steppers[i].currentPosition()) >= 14000) {
            endGame(i);
            break;
        }
    }

    if (resettingGame && !anyStepperMoving()) {
        isGameStarted = false;
        resettingGame = false;
    }
}

void GameControl::resetGame() {
    isGameStarted = false;
    resettingGame = false;
}