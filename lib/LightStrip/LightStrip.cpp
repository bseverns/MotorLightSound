#include "LightStrip.h"

LightStrip::LightStrip() : chaseIndex1(ROW_LENGTH - 1), chaseIndex2(ROW_LENGTH), lastUpdate(0), chaseColor(CRGB::Green), cycles1(0), cycles2(0), buttonPressed(false) {
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
            if (cycles1 < MAX_CYCLES) {
                chase(chaseIndex1, chaseColor, cycles1);
            }
            if (cycles2 < MAX_CYCLES) {
                chase(chaseIndex2, chaseColor, cycles2);
            }
            lastUpdate = currentMillis;

            if (cycles1 >= MAX_CYCLES && cycles2 >= MAX_CYCLES) {
                fill_solid(leds, LED_COUNT, chaseColor);
                FastLED.show();
            }
        }
    }
}

void LightStrip::chase(int& index, CRGB color, int& cycles) {
    if (cycles < MAX_CYCLES) {
        leds[index] = color;

        if (index < ROW_LENGTH) {
            leds[(index + 4) % ROW_LENGTH] = CRGB::Black;
            index--;
            if (index < -4) {
                index = ROW_LENGTH - 1;
                cycles++;
            }
        } else {
            leds[index - 4] = CRGB::Black;
            index++;
            if (index >= LED_COUNT) {
                index = ROW_LENGTH;
                cycles++;
            }
        }

        FastLED.show();
        FastLED.delay(1);
    }
}

void LightStrip::resetCycles() {
    cycles1 = 0;
    cycles2 = 0;
}

void LightStrip::setButtonPressed(bool pressed) {
    buttonPressed = pressed;
}