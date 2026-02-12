#ifndef LAMP_ENGINE_H
#define LAMP_ENGINE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LampEngine {

public:
    void begin(uint16_t startAddress);
    void update(uint8_t* universe, bool signalPresent);

private:
    uint16_t startAddr = 1;

    uint8_t dimmer = 0;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

    Adafruit_NeoPixel* pixel;   // ← WICHTIG
    void applyOutput();
};

#endif
