#include "SerialManager.h"

#include <cctype>

SerialManager::SerialManager(int laneCount, int laneIndPins[])
    : laneCount(laneCount), laneIndPins(laneIndPins) {}

void SerialManager::initialize() {
    for (int i = 0; i < laneCount; ++i) {
        pinMode(laneIndPins[i], OUTPUT);
        digitalWrite(laneIndPins[i], LOW);
    }
    delay(30);
    for (int i = 0; i < laneCount; ++i) {
        digitalWrite(laneIndPins[i], HIGH);
    }
    delay(30);
    for (int i = 0; i < laneCount; ++i) {
        digitalWrite(laneIndPins[i], LOW);
    }
}

void SerialManager::processSerialData(Stream& serial, int laneId) {
    if (!serial.available()) {
        return;
    }

    serial.setTimeout(SERIAL_READ_TIMEOUT_MS);
    String message = serial.readStringUntil('\n');
    message.trim();

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

void SerialManager::setLaneIndicator(int laneId, bool isOk) {
    if (laneId >= 0 && laneId < laneCount) {
        digitalWrite(laneIndPins[laneId], isOk ? HIGH : LOW);
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
