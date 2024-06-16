#include "SerialManager.h"

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
    if (serial.available() > 0) {
        String response = serial.readString();
        if (response.startsWith("SENSOR_OK")) {
            Serial.println("Lane " + String(laneId + 1) + " sensors are OK");
            setLaneIndicator(laneId, true);  // Set corresponding LED on
        } else if (response.startsWith("SENSOR_ERROR")) {
            Serial.println("Lane " + String(laneId + 1) + " sensors reported an error");
            setLaneIndicator(laneId, false);  // Set corresponding LED off
        }

        if (response.length() > 0) {
            char command = response[0];
            handleCommand(laneId, command);
        }
    }
}

void SerialManager::setLaneIndicator(int laneId, bool isOk) {
    if (laneId >= 0 && laneId < laneCount) {
        digitalWrite(laneIndPins[laneId], isOk ? HIGH : LOW);
    }
}

void SerialManager::handleCommand(int laneId, char command) {
    // Handle commands based on the provided laneId and command character
}