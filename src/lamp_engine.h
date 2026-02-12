#ifndef LAMP_ENGINE_H
#define LAMP_ENGINE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class LampEngine {

public:
    void begin(uint16_t startAddress, uint16_t ledCount);
    void update(uint8_t* universe, bool signalPresent);
    void setMode(uint8_t newMode);
    void setStartAddress(uint16_t newAddr);

private:
    uint16_t startAddr = 1;
    uint16_t totalLEDs = 1;

    uint8_t mode = 6;   // 6 Channel Default

    Adafruit_NeoPixel* strip;

    void applyOutput(uint8_t dimmer,
                     uint8_t r,
                     uint8_t g,
                     uint8_t b,
                     uint16_t activeLEDs);
};

#endif
