#include "LEDManager.h"

LEDManager::LEDManager(int ledPin, int ledCount)
    : strip(ledCount, ledPin, NEO_GRB + NEO_KHZ800), ledCount(ledCount) {}

void LEDManager::initialize() {
    strip.begin();
    strip.show();
}

void LEDManager::startChase(uint32_t color) {
    for (int i = 0; i < ledCount / 2; i++) {
        setColor(i, color);
        setColor(ledCount - 1 - i, color);
        show();
        delay(100);
        setColor(i, 0);
        setColor(ledCount - 1 - i, 0);
        show();
    }
}

void LEDManager::setColor(int index, uint32_t color) {
    strip.setPixelColor(index, color);
}

void LEDManager::show() {
    strip.show();
}

uint32_t LEDManager::getColor(uint8_t r, uint8_t g, uint8_t b) {
    return Adafruit_NeoPixel::Color(r, g, b);
}

void LEDManager::clear() {
    strip.clear();
}

void LEDManager::fill(uint32_t color) {
    strip.fill(color, 0, ledCount);
}

uint32_t LEDManager::getLaneColor(int lane) const {
    if (lane >= 0 && lane < 4) {
        return laneColors[lane];
    }
    return 0; // Default to off if lane is out of range
}

const uint32_t LEDManager::laneColors[] = {
    LEDManager::getColor(0, 80, 255),   // Teal - lane 1
    LEDManager::getColor(255, 100, 0),  // Orange-Yellow - lane 2
    LEDManager::getColor(170, 0, 255),  // Purple - lane 3
    LEDManager::getColor(130, 255, 0)   // Green - lane 4
};
