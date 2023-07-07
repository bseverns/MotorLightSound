#include "LightStrip.h"

LightStrip::LightStrip()
    : chaseIndex(ROW_LENGTH - 1),
      lastUpdate(0),
      chaseColor(CRGB::Green),
      cycles(0),
      buttonPressed(false),
      steppersHoming(false) {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, LED_COUNT).setCorrection(TypicalLEDStrip);
}

void LightStrip::begin() {
    FastLED.show();
}

void LightStrip::loop() {
    buttonPressed = (digitalRead(BUTTON_PIN) == LOW);
    unsigned long currentMillis = millis();

    if (buttonPressed) {
        fill_solid(leds, LED_COUNT, CRGB::Red);
        FastLED.show();
    } else {
        if (currentMillis - lastUpdate >= CHASE_DELAY) {
            chase();
            lastUpdate = currentMillis;
        }
    }
}

void LightStrip::chase() {
    int reversedIndex = LED_COUNT - 1 - chaseIndex;  // Reverse the order of the LEDs

    leds[reversedIndex] = chaseColor;

    if (chaseIndex < ROW_LENGTH) {
        leds[(chaseIndex + static_cast<unsigned int>(4)) % ROW_LENGTH] = CRGB::Black;
        chaseIndex--;
        if (chaseIndex < static_cast<unsigned int>(-4)) {
            chaseIndex = ROW_LENGTH - 1;
            cycles++;
        }
    } else {
        leds[(reversedIndex + static_cast<unsigned int>(4)) % LED_COUNT] = CRGB::Black;
        chaseIndex--;
        if (chaseIndex < static_cast<unsigned int>(ROW_LENGTH - 4)) {
            chaseIndex = LED_COUNT - 1;
            cycles++;
        }
    }

    FastLED.show();
    FastLED.delay(1);

    if (cycles >= MAX_CYCLES) {
        resetCycles();
    }
}

void LightStrip::resetCycles() {
    cycles = 0;
}

void LightStrip::setButtonPressed(bool pressed) {
    buttonPressed = pressed;
}

void LightStrip::setSteppersHoming(bool homing) {
    steppersHoming = homing;
}