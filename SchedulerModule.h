#ifndef SCHEDULER_MODULE_H
#define SCHEDULER_MODULE_H

#include <Arduino.h>
#include <time.h>

class SchedulerModule {
public:
    SchedulerModule();
    void begin();
    void update();
    
    // Add a new schedule
    bool addSchedule(uint8_t dayOfWeek, uint8_t startHour, uint8_t startMinute,
                    uint8_t endHour, uint8_t endMinute, bool enableRecording = true, bool enableStreaming = false);
    
    // Remove a schedule
    bool removeSchedule(uint8_t dayOfWeek, uint8_t startHour, uint8_t startMinute);
    
    // Check if recording/streaming should be active
    bool shouldRecord() const { return _shouldRecord; }
    bool shouldStream() const { return _shouldStream; }
    
    // Get duration of current schedule in seconds
    uint32_t getCurrentScheduleDuration() const;
    
private:
    void checkSchedule();
    
    struct Schedule {
        uint8_t dayOfWeek;    // 1 = Sunday, 2 = Monday, ..., 7 = Saturday
        uint8_t startHour;
        uint8_t startMinute;
        uint8_t endHour;
        uint8_t endMinute;
        bool recordingEnabled;
        bool streamingEnabled;
        bool active;
    };
    
    static const uint8_t MAX_SCHEDULES = 10;
    Schedule _schedules[MAX_SCHEDULES];
    uint8_t _scheduleCount;
    bool _shouldRecord;
    bool _shouldStream;
    unsigned long _lastCheckTime;
    uint8_t _currentScheduleIndex;  // Index of the currently active schedule
};

#endif // SCHEDULER_MODULE_H 