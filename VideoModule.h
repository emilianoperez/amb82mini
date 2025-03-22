#ifndef VIDEO_MODULE_H
#define VIDEO_MODULE_H

#include <WiFi.h>
#include "VideoStream.h"
#include "AmebaFatFS.h"
#include "MP4Recording.h"
#include "Config.h"

class VideoModule {
public:
    VideoModule();
    bool begin();
    bool startRecording();
    bool stopRecording();
    bool startStreaming();
    bool stopStreaming();
    bool isRecording() const { return _isRecording; }
    bool isStreaming() const { return _isStreaming; }
    void setRecordingDuration(uint32_t duration) { _mp4.setRecordingDuration(duration); }
    MP4Recording& getMP4Recorder() { return _mp4; }
    VideoSetting& getVideoConfig() { return _videoConfig; }
    
private:
    VideoSetting _videoConfig;
    AmebaFatFS _fs;
    MP4Recording _mp4;
    bool _isRecording;
    bool _isStreaming;
    String _currentFileName;
    void _generateFileName();
    bool _initializeSDCard();
    void _writeFrameToFile();
    void _streamFrame();
};

#endif // VIDEO_MODULE_H 