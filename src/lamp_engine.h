#ifndef LAMP_ENGINE_H
#define LAMP_ENGINE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LampEngine {

public:
    void begin(uint16_t startAddress, uint16_t ledCount);
    void update(uint8_t* universe, bool signalPresent);

private:
    uint16_t startAddr = 1;
    uint16_t totalLEDs = 1;

    Adafruit_NeoPixel* strip;

    void applyOutput(uint8_t dimmer,
                     uint8_t r,
                     uint8_t g,
                     uint8_t b,
                     uint16_t activeLEDs);
};

#endif
