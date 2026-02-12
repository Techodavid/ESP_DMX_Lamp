#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

#include "dmx_receiver.h"
#include "lamp_engine.h"
#include <Adafruit_NeoPixel.h>

// ================= CONFIG =================
#define AP_SSID "ESP32-DMX-DEBUG"
#define AP_PASS "12345678"
#define STATUS_LED_PIN 8
#define STATUS_LED_COUNT 1

Adafruit_NeoPixel statusPixel(STATUS_LED_COUNT, STATUS_LED_PIN, NEO_RGB + NEO_KHZ800);
// ==========================================

class SystemMonitor {
public:
    float getTemperature() {
        return temperatureRead();
    }
};

class WebInterface {

private:
    WebServer server{80};
    DmxReceiver* dmx;
    SystemMonitor* monitor;

public:

    void begin(DmxReceiver* d, SystemMonitor* m) {

        dmx = d;
        monitor = m;

        server.on("/", [this]() { handleRoot(); });
        server.on("/data", [this]() { handleData(); });

        server.begin();
    }

    void loop() {
        server.handleClient();
    }

private:

    void handleRoot() {

        File file = SPIFFS.open("/index.html", "r");

        if (!file) {
            server.send(500, "text/plain", "index.html not found");
            return;
        }

        server.streamFile(file, "text/html");
        file.close();
    }

    void handleData() {

        String json = "{";

        json += "\"frames\":" + String(dmx->frameCount) + ",";
        json += "\"interval\":" + String(dmx->frameInterval) + ",";
        json += "\"signal\":" + String(dmx->signalPresent ? "true" : "false") + ",";
        json += "\"temp\":" + String(monitor->getTemperature()) + ",";

        json += "\"ch\":[";
        for (int i = 2; i <= 512; i++) {
            json += String(dmx->universe[i]);
            if (i < 512) json += ",";
        }
        json += "]";

        json += "}";

        server.send(200, "application/json", json);
    }
};

DmxReceiver dmx;
SystemMonitor monitor;
WebInterface web;
LampEngine lamp;

unsigned long lastBlinkTime = 0;
bool greenPulseActive = false;
bool redBlinkState = false;

void setup() {

    Serial.begin(115200);

    WiFi.softAP(AP_SSID, AP_PASS);

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
    }

    dmx.begin();
    web.begin(&dmx, &monitor);

    statusPixel.begin();
statusPixel.clear();
statusPixel.show();

 lamp.begin(2, 100);    // Startadresse = DMX Channel 1
}

void updateStatusLED() {

    bool dmxOk = dmx.signalPresent;
    bool wifiConnected = WiFi.softAPgetStationNum() > 0;

    unsigned long now = millis();

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    const uint8_t BRIGHT = 20; // Helligkeit LED

    if (dmxOk) {

        // Alle 2 Sekunden kurzer grüner Puls
        if (now - lastBlinkTime > 2000) {
            lastBlinkTime = now;
            greenPulseActive = true;
        }

        if (greenPulseActive) {
            g = BRIGHT;
            if (now - lastBlinkTime > 100) {
                greenPulseActive = false;
            }
        }

    } else {

        // Schnelles rotes Blinken (200ms)
        if (now - lastBlinkTime > 200) {
            lastBlinkTime = now;
            redBlinkState = !redBlinkState;
        }

        if (redBlinkState) {
            r = BRIGHT;
        }
    }

    // WLAN Blau überlagern
    if (wifiConnected) {
        b = BRIGHT;
    }

    statusPixel.setPixelColor(0, statusPixel.Color(r, g, b));
    statusPixel.show();
}


void loop() {

    dmx.loop();
    web.loop();
    lamp.update(dmx.universe, dmx.signalPresent);
     updateStatusLED();
}
