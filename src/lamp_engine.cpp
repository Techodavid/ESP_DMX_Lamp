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
        applyStatic(0, 0, 0, 0, 0);
        return;
    }

    if (mode == 6) {

        uint8_t dimmer = universe[startAddr];
        uint8_t r      = universe[startAddr + 1];
        uint8_t g      = universe[startAddr + 2];
        uint8_t b      = universe[startAddr + 3];
        uint8_t ledCtl = universe[startAddr + 4];
        uint8_t fxValue= universe[startAddr + 5];

        fxSpeed = fxValue;

        uint16_t activeLEDs = map(ledCtl, 0, 255, 0, totalLEDs);

         // FX Bereich bestimmen
        

        if (fxValue <= 50) {
         fxMode = 0;
        }
        else if (fxValue <=100) {
         fxMode = 1;

        // nur Bereich 51–100 normalisieren
          uint8_t local = fxValue - 51;        // 0–49
          fxSpeed = map(local, 0, 49, 5, 255); // Start bei 5
        }
        else if (fxValue <=150)  fxMode = 2;
        else if (fxValue <=200)  fxMode = 3;
        else                     fxMode = 4;

        if (fxMode == 0)
            applyStatic(dimmer, r, g, b, activeLEDs);
        else
            applyFx(dimmer, r, g, b, activeLEDs);
    }

    else if (mode == 3) {

        uint8_t r = universe[startAddr];
        uint8_t g = universe[startAddr + 1];
        uint8_t b = universe[startAddr + 2];

        applyStatic(255, r, g, b, 30);
    }
}

void LampEngine::applyStatic(uint8_t dimmer,
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

void LampEngine::applyFx(uint8_t dimmer,
                         uint8_t r,
                         uint8_t g,
                         uint8_t b,
                         uint16_t activeLEDs)
{
    unsigned long now = millis();
    float norm = fxSpeed / 255.0f;

// exponentielle Kurve
float curve = norm * norm;   // quadratisch

uint16_t delayTime = 200 - (curve * 190); 
// ergibt ca 200ms → 10ms
    

    if (now - lastFxStep < delayTime)
        return;

    lastFxStep = now;

    strip->clear();

    uint32_t color = strip->Color(
        (r * dimmer) / 255,
        (g * dimmer) / 255,
        (b * dimmer) / 255
    );

    switch (fxMode) {

        case 1: // Strobe
        {
            static bool on = false;
            on = !on;

            if (on)
                for (uint16_t i = 0; i < activeLEDs; i++)
                    strip->setPixelColor(i, color);
            break;
        }

        case 2: // Chase right
            strip->setPixelColor(fxPosition % activeLEDs, color);
            fxPosition++;
            break;

        case 3: // Chase left
            strip->setPixelColor(
                (activeLEDs - 1) - (fxPosition % activeLEDs),
                color);
            fxPosition++;
            break;

        case 4: // Wide chase
        {
            uint8_t width = map(fxSpeed, 0, 255, 2, activeLEDs / 2);

            for (uint8_t i = 0; i < width; i++)
                strip->setPixelColor(
                    (fxPosition + i) % activeLEDs,
                    color);

            fxPosition++;
            break;
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
