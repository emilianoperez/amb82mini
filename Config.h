// Configuration file for AMB82-mini
#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_AP_SSID "YourSSID"
#define WIFI_AP_PASS "YourPassword"
#define WIFI_CHANNEL "1"
#define WIFI_HIDDEN 1

// Video Configuration
#define VIDEO_RESOLUTION VIDEO_VGA
#define VIDEO_FPS 10
#define VIDEO_FORMAT VIDEO_H264
#define VIDEO_CHANNEL 0
// RTC Configuration
#define RTC_SPI_FREQUENCY 1000000  // 1MHz SPI clock for RTC
#define RTC_CS_PIN 10  // Chip select pin for RTC

// Power Management
#define MAX_POWER_CONSUMPTION 1.2  // Maximum power consumption in Watts
#define POWER_CHECK_INTERVAL 1000  // Check power consumption every 1 second

// SD Card Configuration
#define SD_CS_PIN SPI_SS
#define VIDEO_FILENAME_PREFIX "VID_"

// RTSP Streaming
#define RTSP_PORT 8554
#define RTSP_STREAM_NAME "stream"

#endif // CONFIG_H 