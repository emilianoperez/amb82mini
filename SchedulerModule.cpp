#include "SchedulerModule.h"
#include "RTCModule.h" // Include RTCModule
#include <time.h>

// Helper function to get day name
const char* getDayName(uint8_t dayOfWeek) {
    switch(dayOfWeek) {
        case 1: return "Monday";
        case 2: return "Tuesday";
        case 3: return "Wednesday";
        case 4: return "Thursday";
        case 5: return "Friday";
        case 6: return "Saturday";
        case 7: return "Sunday";
        default: return "Unknown";
    }
}

SchedulerModule::SchedulerModule() 
    : _scheduleCount(0)
    , _shouldRecord(false)
    , _shouldStream(false)
    , _lastCheckTime(0) {
}

void SchedulerModule::begin() {
    _lastCheckTime = millis();
}

void SchedulerModule::update() {
    unsigned long currentTime = millis();
   
    if (currentTime - _lastCheckTime >= 1000) { // every second
        checkSchedule();
        _lastCheckTime = currentTime;
    }
}

bool SchedulerModule::addSchedule(uint8_t dayOfWeek, uint8_t startHour, uint8_t startMinute,
                                uint8_t endHour, uint8_t endMinute, bool enableRecording, bool enableStreaming) {
    if (_scheduleCount >= MAX_SCHEDULES) return false;
    if (dayOfWeek < 1 || dayOfWeek > 7) return false;
    if (startHour >= 24 || endHour >= 24) return false;
    if (startMinute >= 60 || endMinute >= 60) return false;
    
    _schedules[_scheduleCount] = {
        dayOfWeek,
        startHour,
        startMinute,
        endHour,
        endMinute,
        enableRecording,
        enableStreaming,
        true
    };
    _scheduleCount++;
    return true;
}

bool SchedulerModule::removeSchedule(uint8_t dayOfWeek, uint8_t startHour, uint8_t startMinute) {
    for (uint8_t i = 0; i < _scheduleCount; i++) {
        if (_schedules[i].dayOfWeek == dayOfWeek &&
            _schedules[i].startHour == startHour &&
            _schedules[i].startMinute == startMinute) {
            
            // Move remaining schedules up
            for (uint8_t j = i; j < _scheduleCount - 1; j++) {
                _schedules[j] = _schedules[j + 1];
            }
            _scheduleCount--;
            return true;
        }
    }
    return false;
}

void SchedulerModule::checkSchedule() {
    uint16_t year;
    uint8_t month, day, hour, minute, second;
    rtcModule.getDateTime(year, month, day, hour, minute, second);
    
    // Log current time and day of week
    Serial.print("Current time: ");
    if (hour < 10) Serial.print("0");
    Serial.print(hour);
    Serial.print(":");
    if (minute < 10) Serial.print("0");
    Serial.print(minute);
    Serial.print(":");
    if (second < 10) Serial.print("0");
    Serial.print(second);
    Serial.print(", Day: ");
    
    // Calculate day of week using time.h
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    mktime(&timeinfo);
    
    // Convert to our format (1 = Monday, 2 = Tuesday, ..., 7 = Sunday)
    uint8_t dayOfWeek = timeinfo.tm_wday;
    if (dayOfWeek == 0) dayOfWeek = 7;  // Convert Sunday from 0 to 7
    Serial.println(getDayName(dayOfWeek));
    
    bool shouldRecord = false;
    bool shouldStream = false;
    _currentScheduleIndex = 0xFF;  // Reset current schedule index
    
    // Check each schedule
    for (uint8_t i = 0; i < _scheduleCount; i++) {
        Schedule& schedule = _schedules[i];
        
        // Check if schedule is for current day
        if (schedule.dayOfWeek != dayOfWeek) continue;
        
        // Convert current time to minutes for easier comparison
        uint16_t currentMinutes = hour * 60 + minute;
        uint16_t startMinutes = schedule.startHour * 60 + schedule.startMinute;
        uint16_t endMinutes = schedule.endHour * 60 + schedule.endMinute;
        
        // Handle case where end time is on the next day
        if (endMinutes < startMinutes) {
            endMinutes += 24 * 60;  // Add 24 hours worth of minutes
            if (currentMinutes < startMinutes) {
                currentMinutes += 24 * 60;  // Add 24 hours to current time
            }
        }
        
        // Check if current time is within schedule
        if (currentMinutes >= startMinutes && currentMinutes < endMinutes) {
            shouldRecord = schedule.recordingEnabled;
            shouldStream = schedule.streamingEnabled;
            _currentScheduleIndex = i;  // Set current schedule index
            break;
        }
    }
    
    _shouldRecord = shouldRecord;
    _shouldStream = shouldStream;
}

uint32_t SchedulerModule::getCurrentScheduleDuration() const {
    if (_currentScheduleIndex >= _scheduleCount) return 0;
    
    const Schedule& schedule = _schedules[_currentScheduleIndex];
    
    // Calculate duration in seconds
    uint32_t startSeconds = (schedule.startHour * 3600) + (schedule.startMinute * 60);
    uint32_t endSeconds = (schedule.endHour * 3600) + (schedule.endMinute * 60);
    
    // Handle case where end time is on the next day
    if (endSeconds < startSeconds) {
        endSeconds += 24 * 3600;  // Add 24 hours worth of seconds
    }
    
    return endSeconds - startSeconds;
} 