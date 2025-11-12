#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h>
#include <vector>

class SerialManager {
public:
    static constexpr unsigned long SERIAL_READ_TIMEOUT_MS = 50;

    SerialManager(int laneCount, int laneIndPins[]);
    void initialize();
    void processSerialData(Stream& serial, int laneId);
    void tick(unsigned long now);
    void setLaneIndicator(int laneId, bool isOk);
    void handleCommand(int laneId, char command);

private:
    static constexpr size_t MAX_MESSAGE_LENGTH = 64;
    static constexpr size_t MAX_CHARS_PER_CALL = 8;
    static constexpr unsigned long WARMUP_INTERVAL_MS = 30;

    enum class WarmupState : uint8_t {
        Inactive,
        GoingHigh,
        GoingLow,
        Complete
    };

    void applyWarmupState(bool setHigh);
    void handleMessage(int laneId, const String& message);

    int laneCount;
    int* laneIndPins;
    WarmupState warmupState = WarmupState::Inactive;
    unsigned long warmupDeadline = 0;
    std::vector<String> incomingBuffers;
};

#endif