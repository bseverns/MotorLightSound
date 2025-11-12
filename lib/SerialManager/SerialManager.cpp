#include "SerialManager.h"

#include <cctype>

SerialManager::SerialManager(int laneCount, int laneIndPins[])
    : laneCount(laneCount), laneIndPins(laneIndPins), incomingBuffers(laneCount) {}

void SerialManager::initialize() {
    for (int i = 0; i < laneCount; ++i) {
        pinMode(laneIndPins[i], OUTPUT);
        digitalWrite(laneIndPins[i], LOW);
    }
    warmupState = WarmupState::GoingHigh;
    warmupDeadline = millis() + WARMUP_INTERVAL_MS;
}

void SerialManager::processSerialData(Stream& serial, int laneId) {
    if (laneId < 0 || laneId >= laneCount) {
        return;
    }

    String& buffer = incomingBuffers[laneId];
    size_t processedChars = 0;

    while (serial.available() > 0 && processedChars < MAX_CHARS_PER_CALL) {
        const char incoming = static_cast<char>(serial.read());

        if (incoming == '\r') {
            ++processedChars;
            continue;
        }

        if (incoming == '\n') {
            handleMessage(laneId, buffer);
            buffer = "";
        } else {
            if (buffer.length() >= MAX_MESSAGE_LENGTH) {
                buffer.remove(0, buffer.length() - (MAX_MESSAGE_LENGTH / 2));
            }
            buffer += incoming;
        }

        ++processedChars;
    }
}

void SerialManager::setLaneIndicator(int laneId, bool isOk) {
    if (laneId >= 0 && laneId < laneCount) {
        digitalWrite(laneIndPins[laneId], isOk ? HIGH : LOW);
    }
}

void SerialManager::tick(unsigned long now) {
    switch (warmupState) {
        case WarmupState::Inactive:
        case WarmupState::Complete:
            return;
        case WarmupState::GoingHigh:
            if (now >= warmupDeadline) {
                applyWarmupState(true);
                warmupState = WarmupState::GoingLow;
                warmupDeadline = now + WARMUP_INTERVAL_MS;
            }
            break;
        case WarmupState::GoingLow:
            if (now >= warmupDeadline) {
                applyWarmupState(false);
                warmupState = WarmupState::Complete;
            }
            break;
    }
}

void SerialManager::handleCommand(int laneId, char command) {
    command = static_cast<char>(toupper(command));

    switch (command) {
        case 'R':
            Serial.println("Lane " + String(laneId + 1) + " requested a reset (hook up to GameControl if desired).");
            break;
        case 'S':
            Serial.println("Lane " + String(laneId + 1) + " acknowledged the start signal.");
            break;
        default:
            Serial.println(
                "Lane " + String(laneId + 1) + " sent command '" + String(command) + "' — extend handleCommand() to react.");
            break;
    }
}

void SerialManager::applyWarmupState(bool setHigh) {
    for (int i = 0; i < laneCount; ++i) {
        digitalWrite(laneIndPins[i], setHigh ? HIGH : LOW);
    }
}

void SerialManager::handleMessage(int laneId, const String& message) {
    if (message.length() == 0) {
        return;
    }

    if (message.startsWith("SENSOR_OK")) {
        Serial.println("Lane " + String(laneId + 1) + " sensors are OK");
        setLaneIndicator(laneId, true);
    } else if (message.startsWith("SENSOR_ERROR")) {
        Serial.println("Lane " + String(laneId + 1) + " sensors reported an error");
        setLaneIndicator(laneId, false);
    }

    const int commandIndex = message.lastIndexOf(':');
    const bool hasDelimitedCommand = commandIndex >= 0 && commandIndex < message.length() - 1;

    if (hasDelimitedCommand || message.length() == 1) {
        const char command = hasDelimitedCommand ? message.charAt(commandIndex + 1) : message.charAt(0);
        handleCommand(laneId, command);
    }
}
