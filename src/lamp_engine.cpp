#include "lamp_engine.h"

#define LED_PIN 4     // <-- Dein WS2812 Datenpin
#define MAX_LEDS 255  // Sicherheitslimit

void LampEngine::begin(uint16_t startAddress, uint16_t ledCount) {

    startAddr = startAddress;
    totalLEDs = ledCount;

    if (totalLEDs > MAX_LEDS) totalLEDs = MAX_LEDS;

    strip = new Adafruit_NeoPixel(totalLEDs, LED_PIN, NEO_GRB + NEO_KHZ800);
    strip->begin();
    strip->setBrightness(255);
    strip->clear();
    strip->show();
}

void LampEngine::update(uint8_t* universe, bool signalPresent) {

    if (!signalPresent) {
        applyOutput(0, 0, 0, 0, 0);
        return;
    }

    if (mode == 6) {

        uint8_t dimmer = universe[startAddr];
        uint8_t r      = universe[startAddr + 1];
        uint8_t g      = universe[startAddr + 2];
        uint8_t b      = universe[startAddr + 3];
        uint8_t ledCtl = universe[startAddr + 4];

        uint16_t activeLEDs = map(ledCtl, 0, 255, 0, totalLEDs);

        applyOutput(dimmer, r, g, b, activeLEDs);
    }
    else if (mode == 3) {

        uint8_t r = universe[startAddr];
        uint8_t g = universe[startAddr + 1];
        uint8_t b = universe[startAddr + 2];

        uint8_t dimmer = 255;
        uint16_t activeLEDs = 30;  // FIX

        applyOutput(dimmer, r, g, b, activeLEDs);
    }
}


void LampEngine::applyOutput(uint8_t dimmer,
                             uint8_t r,
                             uint8_t g,
                             uint8_t b,
                             uint16_t activeLEDs)
{
    // Master Dimmer anwenden
    uint8_t rOut = (r * dimmer) / 255;
    uint8_t gOut = (g * dimmer) / 255;
    uint8_t bOut = (b * dimmer) / 255;

    for (uint16_t i = 0; i < totalLEDs; i++) {

        if (i < activeLEDs) {
            strip->setPixelColor(i, strip->Color(rOut, gOut, bOut));
        } else {
            strip->setPixelColor(i, 0);
        }
    }

    strip->show();
}

void LampEngine::setMode(uint8_t newMode) {
    mode = newMode;
}

void LampEngine::setStartAddress(uint16_t newAddr) {
    startAddr = newAddr;
}