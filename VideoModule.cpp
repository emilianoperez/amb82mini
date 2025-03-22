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
    Camera.configVideoChannel(VIDEO_CHANNEL, _videoConfig);
    Camera.videoInit();
    Camera.channelBegin(VIDEO_CHANNEL);
    
    // Initialize SD card
    if (!_initializeSDCard()) {
        return false;
    }
    // Configure MP4 recording
    _mp4.setRecordingFileCount(1);
    _mp4.configVideo(_videoConfig);
    
    return true;
}

bool VideoModule::startRecording() {
    if (_isRecording) return false;
    
    _generateFileName();
    _mp4.setRecordingFileName(_currentFileName);
    _mp4.configVideo(_videoConfig);
    _mp4.begin();
    _isRecording = true;
    return true;
}

bool VideoModule::stopRecording() {
    if (!_isRecording) return false;
    
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
    
    _setupRTSPServer();
    _isStreaming = true;
    return true;
}

bool VideoModule::stopStreaming() {
    if (!_isStreaming) return false;
    
    // TODO: Implement RTSP server stop
    _isStreaming = false;
    return true;
}

void VideoModule::_setupRTSPServer() {
    // TODO: Implement RTSP server setup
    // This will require additional RTSP server library implementation
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
        return false;
    }
    _fs.end();
    return true;
} 