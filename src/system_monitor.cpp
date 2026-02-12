#include <Arduino.h>
#include "system_monitor.h"

float SystemMonitor::getTemperature() {
    return temperatureRead();
}
