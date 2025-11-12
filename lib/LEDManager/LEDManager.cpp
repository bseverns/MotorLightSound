#include "LEDManager.h"

LEDManager::LEDManager(int ledPin, int ledCount)
    : strip(ledCount, ledPin, NEO_GRB + NEO_KHZ800), ledCount(ledCount) {}

void LEDManager::initialize() {
    strip.begin();
    strip.show();
}

void LEDManager::startChase(uint32_t color) {
    chaseColor = color;
    currentChaseIndex = 0;
    chaseLightingPhase = true;
    chaseActive = true;
    lastChaseUpdate = 0;
}

void LEDManager::stopChase() {
    if (!chaseActive) {
        return;
    }

    chaseActive = false;
    chaseLightingPhase = true;
    clear();
    show();
}

void LEDManager::tickChase(unsigned long now) {
    if (!chaseActive) {
        return;
    }

    if (lastChaseUpdate != 0 && now - lastChaseUpdate < CHASE_INTERVAL_MS) {
        return;
    }

    lastChaseUpdate = now;

    const int mirroredIndex = ledCount - 1 - currentChaseIndex;

    if (chaseLightingPhase) {
        setColor(currentChaseIndex, chaseColor);
        setColor(mirroredIndex, chaseColor);
        show();
        chaseLightingPhase = false;
        return;
    }

    setColor(currentChaseIndex, 0);
    setColor(mirroredIndex, 0);
    show();

    currentChaseIndex++;
    chaseLightingPhase = true;

    if (currentChaseIndex >= ledCount / 2) {
        stopChase();
    }
}

bool LEDManager::isChaseActive() const {
    return chaseActive;
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
