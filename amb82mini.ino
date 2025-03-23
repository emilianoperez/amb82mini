#include "Config.h"
#include "RTCModule.h"
#include "VideoModule.h"
#include "PowerManager.h"
#include "SchedulerModule.h"
#include "StreamIO.h"
#include "RTSP.h"
#include "NTPModule.h"

// Global instances of our modules
RTCModule rtcModule;
VideoModule video;
PowerManager power;
SchedulerModule scheduler;
StreamIO videoStreamer(1, 2); 
RTSP rtsp;
NTPModule ntpModule;

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

    // Initialize NTP module
    if (!ntpModule.begin()) {
        Serial.println("Failed to initialize NTP module!");
        while (1) delay(1000);
    }
    
    // Set up recording schedules
    scheduler.begin();
    
    // Configure RTSP with streaming configuration
    rtsp.configVideo(video.getVideoConfig());
    
    // Set up StreamIO pipeline with separate channels
    videoStreamer.registerInput(Camera.getStream(VIDEO_CHANNEL));
    
    // Set up recording pipeline (first output)
    videoStreamer.registerOutput1(video.getMP4Recorder());
    
    // Set up streaming pipeline (second output)
    videoStreamer.registerOutput2(rtsp);
    
    // Start the StreamIO pipeline
    if (videoStreamer.begin() != 0) {
        Serial.println("StreamIO link start failed");
    }

    power.setWiFiEnabled(true);
    delay(1000);

    // Try to sync time with NTP server
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi Connected!");
        if (ntpModule.syncTime()) {
            Serial.println("Time synchronized with NTP server");
            // Get and print the current time
            uint16_t year;
            uint8_t month, day, hour, minute, second;
            rtcModule.getDateTime(year, month, day, hour, minute, second);
            
            Serial.print("Current time: ");
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
            Serial.print(minute);
            Serial.print(":");
            if (second < 10) Serial.print("0");
            Serial.println(second);

            // Get current day of week (1=Monday, 2=Tuesday, ..., 7=Sunday)
            uint8_t currentDay = rtcModule.getDayOfWeek();
            Serial.print("Current day of week: ");
            Serial.println(currentDay);

            // Set up schedules for the current day
            //streaming only
            scheduler.addSchedule(currentDay, hour, minute, hour, minute +1, false, true);
            //streaming and recording
            scheduler.addSchedule(currentDay, hour, minute +1, hour, minute +2, true, true);
            //recording only
            scheduler.addSchedule(currentDay, hour, minute +3, hour, minute +6, true, false);
            //streaming only
            scheduler.addSchedule(currentDay, hour, minute +7, hour, minute +14, false, true);

        } else {
            Serial.println("Failed to sync time with NTP server");
            // Set a default time as fallback
            rtcModule.setDateTime(2025, 3, 18, 16, 0, 0);

            // Tue (2) from 16:00 to 16:01 - streaming only
            scheduler.addSchedule(2, 16, 0, 16, 1, false, true);
            // Tue (2) from 16:01 to 16:02 - streaming and recording
            scheduler.addSchedule(2, 16, 1, 16, 2, true, true);
            // Tue (2) from 16:03 to 16:06 - recording only
            scheduler.addSchedule(2, 16, 3, 16, 6, true, false);
        }
    } else {
        Serial.println("WiFi not connected, using default time");
        rtcModule.setDateTime(2025, 3, 18, 16, 0, 0);

         // Tue (2) from 16:00 to 16:01 - streaming only
        scheduler.addSchedule(2, 16, 0, 16, 1, false, true);
        // Tue (2) from 16:01 to 16:02 - streaming and recording
        scheduler.addSchedule(2, 16, 1, 16, 2, true, true);
        // Tue (2) from 16:03 to 16:06 - recording only
        scheduler.addSchedule(2, 16, 3, 16, 6, true, false);
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
                 // Start RTSP streaming
                rtsp.begin();
                Serial.println("RTSP streaming started");
                isStreaming = true;
                // Print RTSP URL
                IPAddress ip = WiFi.localIP();
                Serial.print("RTSP URL: rtsp://");
                Serial.print(ip);
                Serial.print(":");
                Serial.println(rtsp.getPort());
            }
        }
    } else if (!shouldBeStreaming && isStreaming) {
        rtsp.end();
        if (video.stopStreaming()) {
            Serial.println("RTSP streaming stopped");
            isStreaming = false;
            power.setWiFiEnabled(false);
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