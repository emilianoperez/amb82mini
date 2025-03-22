#include "RTCModule.h"


RTCModule::RTCModule() : _isRunning(false), _lastEpoch(0), _timeinfo(nullptr) {
}

bool RTCModule::begin() {
    rtc.Init();
    _isRunning = true;
    return true;
}

bool RTCModule::setDateTime(uint16_t year, uint8_t month, uint8_t day,
                          uint8_t hour, uint8_t minute, uint8_t second) {
    if (!_isRunning) return false;
    
    _lastEpoch = rtc.SetEpoch(year, month, day, hour, minute, second);
    rtc.Write(_lastEpoch);
    
    return true;
}

void RTCModule::getDateTime(uint16_t &year, uint8_t &month, uint8_t &day,
                          uint8_t &hour, uint8_t &minute, uint8_t &second) {
    if (!_isRunning) return;
    
    long long currentEpoch = rtc.Read();
    _timeinfo = localtime(&currentEpoch);
    
    year = _timeinfo->tm_year + 1900;
    month = _timeinfo->tm_mon + 1;
    day = _timeinfo->tm_mday;
    hour = _timeinfo->tm_hour;
    minute = _timeinfo->tm_min;
    second = _timeinfo->tm_sec;
}

uint8_t RTCModule::getDayOfWeek() {
    struct tm* timeinfo;
    time_t now = rtc.Read();
    timeinfo = localtime(&now);
    return timeinfo->tm_wday; 
}

bool RTCModule::setAlarm(uint8_t hour, uint8_t minute) {
    if (!_isRunning) return false;
    
    // Get current time
    long long currentEpoch = rtc.Read();
    _timeinfo = localtime(&currentEpoch);
    
    // Set alarm time
    struct tm alarmTime = *_timeinfo;
    alarmTime.tm_hour = hour;
    alarmTime.tm_min = minute;
    alarmTime.tm_sec = 0;
    
    rtc.EnableAlarm(alarmTime.tm_mday, hour, minute, 0, nullptr);
    
    return true;
}

void RTCModule::clearAlarm() {
    if (!_isRunning) return;
    rtc.DisableAlarm();
}

bool RTCModule::isAlarmTriggered() {
    if (!_isRunning) return false;
    // Note: The RTC class doesn't have a direct method to check if alarm is triggered
    // We'll need to implement this differently
    return false;
} 