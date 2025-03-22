#include "Config.h"
#include "RTCModule.h"
#include "VideoModule.h"
#include "PowerManager.h"
#include "SchedulerModule.h"
#include "StreamIO.h"

// Global instances of our modules
RTCModule rtcModule;
VideoModule video;
PowerManager power;
SchedulerModule scheduler;
StreamIO videoStreamer(1, 1);    // 1 Input Video -> 1 Output MP4

void setup() {
    Serial.begin(115200);
    Serial.println("AMB82-mini Camera System Starting...");
    
    // Initialize modules
    if (!rtcModule.begin()) {
        Serial.println("Failed to initialize RTC!");
        while (1) delay(1000);
    }
    
    if (!video.begin()) {
        Serial.println("Failed to initialize Video!");
        while (1) delay(1000);
    }
    
    if (!power.begin()) {
        Serial.println("Failed to initialize Power Manager!");
        while (1) delay(1000);
    }
    
    // Set initial date/time (should be done only once)
    // In production, this should be done through a configuration interface
    rtcModule.setDateTime(2025, 3, 18, 16, 0, 0);
    
    // Set up recording schedules
    scheduler.begin();
    
    // Example schedules:
    // Tue (2) from 16:00 to 16:01 - streaming only
    scheduler.addSchedule(2, 16, 0, 16, 1, false, true);
    // Tue (2) from 16:01 to 16:02 - streaming and recording
    scheduler.addSchedule(2, 16, 1, 16, 2, true, true);

    // Tue (2) from 16:03 to 16:06 - recording only
    scheduler.addSchedule(2, 16, 3, 16, 6, true, false);
    
    // Configure StreamIO pipeline for video recording
    videoStreamer.registerInput(Camera.getStream(VIDEO_CHANNEL));
    videoStreamer.registerOutput(video.getMP4Recorder());
    if (videoStreamer.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }
    
    Serial.println("System initialized successfully!");
}

void loop() {
    static bool isRecording = false;
    static bool isStreaming = false;
    static unsigned long lastStatusTime = 0;
    uint16_t year;
    uint8_t month, day, hour, minute, second;
    
    // Get current time
    rtcModule.getDateTime(year, month, day, hour, minute, second);
    
    // Update scheduler
    scheduler.update();
    
    // Check if we should start/stop recording based on schedule
    bool shouldBeRecording = scheduler.shouldRecord();
    bool shouldBeStreaming = scheduler.shouldStream();
    
    // Handle recording state
    if (shouldBeRecording && !isRecording) {

        // Calculate duration in seconds from current schedule
        uint32_t duration = scheduler.getCurrentScheduleDuration();
        video.setRecordingDuration(duration);
        
        if (video.startRecording()) {
            Serial.println("Recording started");
            isRecording = true;
        }
    } else if (!shouldBeRecording && isRecording) {
        if (video.stopRecording()) {
            Serial.println("Recording stopped");
            isRecording = false;
        }
    }
    
    // Handle streaming state
    if (shouldBeStreaming && !isStreaming) {
        if (power.isPowerOK()) {
            power.setWiFiEnabled(true);
            if (video.startStreaming()) {
                Serial.println("Streaming started");
                isStreaming = true;
            }
        }
    } else if (!shouldBeStreaming && isStreaming) {
        if (video.stopStreaming()) {
            Serial.println("Streaming stopped");
            isStreaming = false;
            // Only disable WiFi if we're not recording
            if (!isRecording) {
                power.setWiFiEnabled(false);
            }
        }
    }
    
    // Monitor and optimize power consumption
    float currentPower = power.getCurrentPower();
    if (!power.isPowerOK()) {
        Serial.print("Power consumption too high: ");
        Serial.print(currentPower);
        Serial.println("W");
        power.optimizePower();
    }
    
    // Print status every minute
    unsigned long currentTime = millis();
    if (currentTime - lastStatusTime >= 30000) {  // Every half minute
        printStatus(year, month, day, hour, minute, currentPower);
        lastStatusTime = currentTime;
    }
}

void printStatus(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, float power) {
    // Print time
    Serial.print("Time: ");
    Serial.print(year);
    Serial.print("-");
    if (month < 10) Serial.print("0");
    Serial.print(month);
    Serial.print("-");
    if (day < 10) Serial.print("0");
    Serial.print(day);
    Serial.print(" ");
    if (hour < 10) Serial.print("0");
    Serial.print(hour);
    Serial.print(":");
    if (minute < 10) Serial.print("0");
    Serial.println(minute);
    
    // Print recording status
    Serial.print("Recording: ");
    Serial.println(video.isRecording() ? "Yes" : "No");
    
    // Print streaming status
    Serial.print("Streaming: ");
    Serial.println(video.isStreaming() ? "Yes" : "No");
    
    // Print power consumption
    Serial.print("Power: ");
    Serial.print(power, 2);
    Serial.println("W");
} 