#pragma once

#include <Arduino.h>

// Debug tags
#define SETUP_LOG_TAG "SETUP"
#define BUTTON_LOG_TAG "BUTTON"
#define SCREEN_LOG_TAG "SCREEN"

#define WEATHER_REQUEST_LOG_TAG "WEATHER_REQUEST"
#define TIME_REQUEST_LOG_TAG "TIME_REQUEST"
#define PATIENT_DATA_SEND_LOG_TAG "PATIENT_DATA_SEND"

#define WEATHER_SCREEN_LOG_TAG "WEATHER_SCREEN"
#define HOME_SCREEN_LOG_TAG "HOME_SCREEN"
#define HEART_SCREEN_LOG_TAG "HEART_SCREEN"

#define TOPBAR_WEATHER_LOG_TAG "TOPBAR_WEATHER"

#define WHO_AM_I_LOG_TAG "WHO_AM_I"
#define BATTERY_LOG_TAG "BATTERY"

// To be monitored tags (INFO)
#define HEART_SENSOR_LOG_TAG "HEART_SENSOR"
#define HEART_SENSOR_SPO2_LOG_TAG "HEART_SENSOR_SPO2"
#define HEART_SENSOR_TEMPERATURE_LOG_TAG "HEART_SENSOR_TEMPERATURE"
#define HEART_SENSOR_HR_LOG_TAG "HEART_SENSOR_HR"
#define HEART_SENSOR_IR_LOG_TAG "HEART_SENSOR_IR"

enum log_level {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_VERBOSE
};

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif

class logger {
public:
    static void log(int level, const char* tag, const char* file, int line, const char* func, const char* format, ...);
};

#define LOG_ERROR(tag, fmt, ...)    logger::log(LOG_LEVEL_ERROR, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(tag, fmt, ...)  logger::log(LOG_LEVEL_WARNING, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(tag, fmt, ...)     logger::log(LOG_LEVEL_INFO, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(tag, fmt, ...)    logger::log(LOG_LEVEL_DEBUG, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_VERBOSE(tag, fmt, ...)  logger::log(LOG_LEVEL_VERBOSE, tag, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
