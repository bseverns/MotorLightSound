#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h>

class SerialManager {
public:
    static constexpr unsigned long SERIAL_READ_TIMEOUT_MS = 50;

    SerialManager(int laneCount, int laneIndPins[]);
    void initialize();
    void processSerialData(Stream& serial, int laneId);
    void setLaneIndicator(int laneId, bool isOk);
    void handleCommand(int laneId, char command);

private:
    int laneCount;
    int* laneIndPins;
};

#endif