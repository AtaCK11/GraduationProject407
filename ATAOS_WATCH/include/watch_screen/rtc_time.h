#pragma once

#include "logger/logger.h"

typedef struct {
    char day_name[32];
    char date[32];
    int hour;
    int minute;
    int second;
} TimeInformation;

typedef struct {
    char requestType[5];  // Example: "Time"
} TimeRequest;

class rtc_time {
public:
    void request_time(void *pvParameters);
    void calculate_time(void *pvParameters);
    
    TimeInformation received_time;
    TimeRequest requested_time;

    const char* dayNames[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    // write a default constructor for received_weather and requested_weather
    rtc_time() {
        strcpy(received_time.day_name, "Sali");
        strcpy(received_time.date, "10.06.2025");
        received_time.hour = 13;
        received_time.minute = 30;
        received_time.second = 31;

        strcpy(requested_time.requestType, "Time");
    }


private:
    
};
