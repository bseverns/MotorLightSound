#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Adafruit_NeoPixel.h>

class LEDManager {
public:
    LEDManager(int ledPin, int ledCount);
    void initialize();
    void startChase(uint32_t color);
    void stopChase();
    void tickChase(unsigned long now);
    bool isChaseActive() const;
    void setColor(int index, uint32_t color);
    void show();
    static uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
    void clear();
    void fill(uint32_t color);
    uint32_t getLaneColor(int lane) const;

private:
    Adafruit_NeoPixel strip;
    int ledCount;
    uint32_t chaseColor = 0;
    unsigned long lastChaseUpdate = 0;
    int currentChaseIndex = 0;
    bool chaseActive = false;
    bool chaseLightingPhase = true;
    static constexpr unsigned long CHASE_INTERVAL_MS = 100;
    static const uint32_t laneColors[];
};

#endif