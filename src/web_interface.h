#pragma once

#include <WebServer.h>
#include "dmx_receiver.h"
#include "lamp_engine.h"

#include "system_monitor.h"

class WebInterface {

private:
    WebServer server{80};
    DmxReceiver* dmx;
    SystemMonitor* monitor;
    LampEngine* lamp;

public:
    void begin(DmxReceiver* d, SystemMonitor* m, LampEngine* l);
    void loop();

private:
    void handleRoot();
    void handleData();
    void handleGetConfig();
    void handleSetConfig();
};
