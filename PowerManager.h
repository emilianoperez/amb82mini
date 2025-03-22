#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include "Config.h"
#include <WiFi.h>

class PowerManager {
public:
    PowerManager();
    bool begin();
    float getCurrentPower();
    bool isPowerOK() const;
    void optimizePower();
    void setWiFiEnabled(bool enabled);
    
private:
    float _currentPower;
    unsigned long _lastCheckTime;
    bool _wifiEnabled;
    
    void _measurePower();
    void _updatePowerStatus();
    bool _checkPowerThreshold();
};

#endif // POWER_MANAGER_H 