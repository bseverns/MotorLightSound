#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

#include <FastLED.h>

#define LED_PIN 23
#define LED_COUNT 42
#define ROW_LENGTH 21
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define CHASE_DELAY 55
#define MAX_CYCLES 3

class LightStrip {
private:
    CRGB leds[LED_COUNT];
    unsigned int chaseIndex;
    uint32_t lastUpdate;
    CRGB chaseColor;
    int cycles;
    bool buttonPressed;
    bool steppersHoming;

public:
    LightStrip();

    void begin();

    void loop();

    void setChaseColor(const CRGB& color);

    void setButtonPressed(bool pressed);

    void setSteppersHoming(bool homing);

private:
   void chase(unsigned int& index);

    void resetCycles();
};

#endif // LIGHTSTRIP_H