#include "LightStrip.h"

LightStrip::LightStrip() : chaseIndex(ROW_LENGTH - 1), lastUpdate(0), chaseColor(CRGB::Green), cycles(0), buttonPressed(false), steppersHoming(false) {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
}

void LightStrip::begin() {
    FastLED.show();
}

void LightStrip::loop() {
    unsigned long currentMillis = millis();

    if (buttonPressed) {
        fill_solid(leds, LED_COUNT, CRGB::Red);
        FastLED.show();
    } else if (steppersHoming) {
        fill_solid(leds, LED_COUNT, CRGB::Blue);
        FastLED.show();
    } else {
        if (currentMillis - lastUpdate >= CHASE_DELAY) {
            if (cycles < MAX_CYCLES) {
                chase(chaseIndex);
            }
            lastUpdate = currentMillis;

            if (cycles >= MAX_CYCLES) {
                fill_solid(leds, LED_COUNT, chaseColor);
                FastLED.show();
            }
        }
    }
}

void LightStrip::chase(int& chaseIndex1, int& chaseIndex2, CRGB color, int& cycles) {
    if (cycles < MAX_CYCLES) {
        int reversedIndex1 = LED_COUNT - 1 - chaseIndex1;  // Reverse the order of the LEDs in row 1
        int reversedIndex2 = LED_COUNT - 1 - chaseIndex2;  // Reverse the order of the LEDs in row 2

        leds[reversedIndex1] = color;
        leds[reversedIndex2] = color;

        FastLED.show();
        FastLED.delay(1);

        leds[reversedIndex1] = CRGB::Black;
        leds[reversedIndex2] = CRGB::Black;

        chaseIndex1--;
        if (chaseIndex1 < 0) {
            chaseIndex1 = ROW_LENGTH - 1;
        }

        chaseIndex2--;
        if (chaseIndex2 < ROW_LENGTH) {
            chaseIndex2 = LED_COUNT - 1;
            cycles++;
        }
    }
}

void LightStrip::resetCycles() {
    cycles = 0;
}

void LightStrip::setChaseColor(const CRGB& color) {
    chaseColor = color;
}

void LightStrip::setButtonPressed(bool pressed) {
    buttonPressed = pressed;
}

void LightStrip::setSteppersHoming(bool homing) {
    steppersHoming = homing;
}