#include "NTPModule.h"
#include "RTCModule.h"
#include <WiFiUdp.h>

NTPModule::NTPModule()
    : _isTimeSynced(false)
    , _ntpServer("pool.ntp.org")
    , _ntpPort(123)
    , _timeZone(0)  // UTC
    , _dst(0)       // No daylight saving
{
}

bool NTPModule::begin() {
    return true;
}

bool NTPModule::syncTime() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Cannot sync time.");
        return false;
    }

    time_t epoch;
    if (!_getNTPTime(epoch)) {
        Serial.println("Failed to get time from NTP server");
        return false;
    }

    // Convert epoch to date/time components
    struct tm* timeinfo = localtime(&epoch);
    uint16_t year = timeinfo->tm_year + 1900;
    uint8_t month = timeinfo->tm_mon + 1;
    uint8_t day = timeinfo->tm_mday;
    uint8_t hour = timeinfo->tm_hour;
    uint8_t minute = timeinfo->tm_min;
    uint8_t second = timeinfo->tm_sec;

    // Set RTC with NTP time
    if (rtcModule.setDateTime(year, month, day, hour, minute, second)) {
        _isTimeSynced = true;
        Serial.println("Time synchronized successfully");
        return true;
    }

    Serial.println("Failed to set RTC time");
    return false;
}

bool NTPModule::_getNTPTime(time_t& epoch) {
    WiFiUDP udp;
    if (!udp.begin(8888)) {
        Serial.println("Failed to start UDP");
        return false;
    }

    // Send NTP request
    uint8_t packetBuffer[48];
    memset(packetBuffer, 0, 48);
    
    // Initialize values needed to form NTP request
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;            // Stratum, or type of clock
    packetBuffer[2] = 6;            // Polling Interval
    packetBuffer[3] = 0xEC;         // Peer Clock Precision
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    udp.beginPacket(_ntpServer, _ntpPort);
    udp.write(packetBuffer, 48);
    udp.endPacket();

    // Wait for response
    unsigned long startTime = millis();
    while (udp.parsePacket() == 0) {
        if (millis() - startTime > 5000) {
            udp.stop();
            return false;
        }
    }

    // Read the response
    udp.read(packetBuffer, 48);
    udp.stop();

    // Extract time from response
    uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);
    uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);
    uint32_t secsSince1900 = highWord << 16 | lowWord;
    epoch = secsSince1900 - 2208988800UL; // Convert to Unix epoch

    return true;
} 