#ifndef NTP_MODULE_H
#define NTP_MODULE_H

#include <WiFi.h>
#include "Config.h"

class NTPModule {
public:
    NTPModule();
    bool begin();
    bool syncTime();
    bool isTimeSynced() const { return _isTimeSynced; }
    
private:
    bool _isTimeSynced;
    bool _getNTPTime(time_t& epoch);
    const char* _ntpServer;
    const int _ntpPort;
    const int _timeZone;
    const int _dst;
};

extern NTPModule ntpModule;

#endif // NTP_MODULE_H 