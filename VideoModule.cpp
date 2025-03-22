#include "VideoModule.h"
#include "RTCModule.h"
#include <time.h>

VideoModule::VideoModule() 
    : _videoConfig(VIDEO_RESOLUTION, VIDEO_FPS, VIDEO_FORMAT, 1),
      _isRecording(false),
      _isStreaming(false){
}

bool VideoModule::begin() {
    // Initialize camera
    _videoConfig.setBitrate(2 * 1024 * 1024);
    Camera.configVideoChannel(VIDEO_CHANNEL, _videoConfig);
    Camera.videoInit();
    Camera.channelBegin(VIDEO_CHANNEL);
    
    // Initialize SD card
    if (!_initializeSDCard()) {
        Serial.println("Failed to initialize SD card!");
        return false;
    }
    Serial.println("SD card initialized successfully");
    
    // Configure MP4 recording
    _mp4.setRecordingFileCount(1);
    _mp4.configVideo(_videoConfig);
    
    Serial.println("MP4 recorder initialized");
    
    return true;
}

bool VideoModule::startRecording() {
    if (_isRecording) {
        Serial.println("Recording already in progress");
        return false;
    }
    
    _generateFileName();
    Serial.print("Starting recording to file: ");
    Serial.println(_currentFileName);
    
    _mp4.setRecordingFileName(_currentFileName);
    _mp4.configVideo(_videoConfig);
    
    // Start the recording
    _mp4.begin();
    _isRecording = true;
    return true;
}

bool VideoModule::stopRecording() {
    if (!_isRecording) {
        Serial.println("No recording in progress");
        return false;
    }
    _mp4.end();
    _isRecording = false;
    return true;
}

bool VideoModule::startStreaming() {
    if (_isStreaming) return false;
    
    // Setup WiFi AP if not already done
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.apbegin(WIFI_AP_SSID, WIFI_AP_PASS, WIFI_CHANNEL, WIFI_HIDDEN);
        delay(1000); // Wait for AP to start
    }
    
    _isStreaming = true;
    return true;
}

bool VideoModule::stopStreaming() {
    if (!_isStreaming) return false;
    _isStreaming = false;
    return true;
}

void VideoModule::_generateFileName() {
    uint16_t year;
    uint8_t month, day, hour, minute, second;

    // Get current time from RTC
    rtcModule.getDateTime(year, month, day, hour, minute, second);

    char buffer[32];
    sprintf(buffer, "%s%04d%02d%02d_%02d%02d%02d",
            VIDEO_FILENAME_PREFIX,
            year,
            month,
            day,
            hour,
            minute,
            second);
    
    _currentFileName = String(buffer);
}

bool VideoModule::_initializeSDCard() {
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    
    if (!_fs.begin()) {
        Serial.println("Failed to mount SD card");
        return false;
    }
    Serial.println("SD card mounted successfully");
    return true;
} 