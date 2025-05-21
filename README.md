# AMB82-mini Camera System

A comprehensive camera system built for the AMB82-mini platform that supports video recording, RTSP streaming, and scheduled operations.

## Features

- Video Recording with MP4 format
- RTSP Streaming
- Scheduled Recording and Streaming
- Real-Time Clock (RTC) Support
- Network Time Protocol (NTP) Synchronization
- Power Management
- WiFi Access Point Mode

## Hardware Requirements

- AMB82-mini Development Board
- SD Card (for video storage)
- RTC Module
- Camera Module compatible with AMB82-mini

## Software Dependencies

- Arduino IDE or PlatformIO
- Required Libraries:
  - WiFi
  - WiFiUDP
  - AmebaFatFS
  - VideoStream
  - MP4Recording
  - RTSP

## Configuration

The system can be configured through `Config.h`. Key configuration parameters include:

### WiFi Configuration
```cpp
#define WIFI_AP_SSID "YourSSID"
#define WIFI_AP_PASS "YourPassword"
#define WIFI_CHANNEL "1"
#define WIFI_HIDDEN 1
```

### Video Configuration
```cpp
#define VIDEO_RESOLUTION VIDEO_VGA
#define VIDEO_FPS 10
#define VIDEO_FORMAT VIDEO_H264
#define VIDEO_CHANNEL 0
```

### Power Management
```cpp
#define MAX_POWER_CONSUMPTION 1.2  // Maximum power consumption in Watts
#define POWER_CHECK_INTERVAL 1000  // Check power consumption every 1 second
```

### RTSP Streaming
```cpp
#define RTSP_PORT 8554
#define RTSP_STREAM_NAME "stream"
```

## Module Overview

### 1. VideoModule
Handles video recording and streaming functionality:
- MP4 recording with configurable duration
- RTSP streaming
- SD card management
- File naming with timestamps

### 2. RTCModule
Manages real-time clock operations:
- Time synchronization
- Date/time retrieval
- Alarm functionality
- Day of week calculation

### 3. NTPModule
Provides network time synchronization:
- NTP server communication
- Time synchronization with RTC
- Automatic time updates

### 4. PowerManager
Monitors and optimizes power consumption:
- Power usage monitoring
- WiFi power management
- Automatic power optimization
- Power threshold monitoring

### 5. SchedulerModule
Manages scheduled operations:
- Multiple schedule support
- Recording and streaming scheduling
- Time-based operations
- Schedule duration calculation

## Usage

### Basic Setup
1. Connect the hardware components
2. Configure WiFi settings in `Config.h`
3. Upload the code to AMB82-mini
4. Power on the system

### Recording
The system can record video in two ways:
1. Manual recording through API calls
2. Scheduled recording based on configured schedules

### Streaming
RTSP streaming can be accessed using:
```
rtsp://<device-ip>:8554/stream
```

### Scheduling
Schedules can be added using the following format:
```cpp
scheduler.addSchedule(
    dayOfWeek,    // 1=Monday to 7=Sunday
    startHour,    // 0-23
    startMinute,  // 0-59
    endHour,      // 0-23
    endMinute,    // 0-59
    enableRecording,
    enableStreaming
);
```

## Power Management

The system includes automatic power management:
- Monitors power consumption
- Disables WiFi when power consumption is high
- Optimizes operations based on available power

## File Structure

```
├── amb82mini.ino        # Main program file
├── Config.h            # Configuration settings
├── RTCModule.h/cpp     # RTC functionality
├── VideoModule.h/cpp   # Video handling
├── NTPModule.h/cpp     # Time synchronization
├── PowerManager.h/cpp  # Power management
└── SchedulerModule.h/cpp # Scheduling system
```

## Error Handling

The system includes comprehensive error handling:
- SD card initialization checks
- WiFi connection monitoring
- Power consumption warnings
- Recording/streaming state validation

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

For support, please open an issue in the repository or contact the maintainers. 