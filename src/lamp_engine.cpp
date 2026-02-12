#include "lamp_engine.h"

#define NEOPIXEL_PIN 9
#define NUM_PIXELS 1

void LampEngine::begin(uint16_t startAddress) {

    startAddr = startAddress;

    pixel = new Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
    pixel->begin();
    pixel->clear();
    pixel->show();
}

void LampEngine::update(uint8_t* universe, bool signalPresent) {

    if (!signalPresent) {
        dimmer = red = green = blue = 0;
        applyOutput();
        return;
    }

    dimmer = universe[startAddr];
    red    = universe[startAddr + 1];
    green  = universe[startAddr + 2];
    blue   = universe[startAddr + 3];

    applyOutput();
}

void LampEngine::applyOutput() {

    // Master Dimmer anwenden
    uint8_t r = (red   * dimmer) / 255;
    uint8_t g = (green * dimmer) / 255;
    uint8_t b = (blue  * dimmer) / 255;

    pixel->setPixelColor(0, pixel->Color(r, g, b));
    pixel->show();
}
