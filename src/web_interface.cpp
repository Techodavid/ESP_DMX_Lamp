#include "web_interface.h"
#include <SPIFFS.h>
#include <Preferences.h>

extern Preferences prefs;
extern uint16_t dmxStartAddress;
extern uint8_t lampMode;

void WebInterface::begin(DmxReceiver* d, SystemMonitor* m, LampEngine* l) {

    dmx = d;
    monitor = m;
    lamp = l;

    server.on("/", [this]() { handleRoot(); });
    server.on("/data", [this]() { handleData(); });
    server.on("/config", HTTP_GET, [this]() { handleGetConfig(); });
    server.on("/config", HTTP_POST, [this]() { handleSetConfig(); });

    server.begin();
}

void WebInterface::loop() {
    server.handleClient();
}

void WebInterface::handleRoot() {

    File file = SPIFFS.open("/index.html", "r");

    if (!file) {
        server.send(500, "text/plain", "index.html not found");
        return;
    }

    server.streamFile(file, "text/html");
    file.close();
}

void WebInterface::handleData() {

    bool sendChannels = server.hasArg("ch");

    String json = "{";

    json += "\"frames\":" + String(dmx->frameCount) + ",";
    json += "\"interval\":" + String(dmx->frameInterval) + ",";
    json += "\"signal\":" + String(dmx->signalPresent ? "true" : "false") + ",";
    json += "\"temp\":" + String(monitor->getTemperature()) + ",";
    json += "\"mode\":" + String(lampMode);

    if (sendChannels) {

        json += ",\"ch\":[";

        for (int i = 2; i <= 512; i++) {
            json += String(dmx->universe[i]);
            if (i < 512) json += ",";
        }

        json += "]";
    }

    json += "}";

    server.send(200, "application/json", json);
}

void WebInterface::handleGetConfig() {

    String json = "{";
    json += "\"start\":" + String(dmxStartAddress - 1) + ",";
    json += "\"mode\":" + String(lampMode);
    json += "}";

    server.send(200, "application/json", json);
}

void WebInterface::handleSetConfig() {

    bool changed = false;

    uint8_t newMode = lampMode;
    uint16_t newStart = dmxStartAddress;

    if (server.hasArg("mode")) {

        newMode = server.arg("mode").toInt();
        if (newMode != 3 && newMode != 6)
            newMode = 6;

        changed = true;
    }

    if (server.hasArg("start")) {

        newStart = server.arg("start").toInt();
        newStart += 1;

        if (newStart < 1) newStart = 1;
        if (newStart > 512) newStart = 512;

        changed = true;
    }

    if (!changed) {
        server.send(400, "text/plain", "No valid parameter");
        return;
    }

    server.send(200, "text/plain", "OK");

    lampMode = newMode;
    dmxStartAddress = newStart;

    prefs.putUChar("mode", lampMode);
    prefs.putUShort("start", dmxStartAddress);

    lamp->setMode(lampMode);
    lamp->setStartAddress(dmxStartAddress);
}
