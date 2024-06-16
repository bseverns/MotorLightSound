#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Adafruit_NeoPixel.h>

class LEDManager {
public:
    LEDManager(int ledPin, int ledCount);
    void initialize();
    void startChase(uint32_t color);
    void setColor(int index, uint32_t color);
    void show();
    static uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
    void clear();
    void fill(uint32_t color);
    uint32_t getLaneColor(int lane) const;

private:
    Adafruit_NeoPixel strip;
    int ledCount;
    static const uint32_t laneColors[];
};

#endif