#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <time.h>
#include "rtc.h"
#include "Config.h"

class RTCModule {
public:
    RTCModule();
    bool begin();
    bool setDateTime(uint16_t year, uint8_t month, uint8_t day, 
                    uint8_t hour, uint8_t minute, uint8_t second);
    void getDateTime(uint16_t &year, uint8_t &month, uint8_t &day,
                    uint8_t &hour, uint8_t &minute, uint8_t &second);
    uint8_t getDayOfWeek();  // Returns 1=Monday, 2=Tuesday, ..., 7=Sunday
    bool setAlarm(uint8_t hour, uint8_t minute);
    void clearAlarm();
    bool isAlarmTriggered();
    
private:
    bool _isRunning;
    long long _lastEpoch;
    struct tm *_timeinfo;
};

extern RTCModule rtcModule;

#endif // RTC_MODULE_H 