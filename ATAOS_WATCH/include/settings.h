#pragma once

// Request Timers
#define SERVER_TIME_REQUEST_TIMER 3600000
#define SERVER_WEATHER_REQUEST_TIMER 1800000

#define WATCH_SCREEN_PRESS_RESPONSE_TIMER 50

#define WATCH_SCREEN_REFRESH_TIMER 100

// Heart Sensor Timers
#define HEART_SENSOR_TIMER 2 //20
#define HEART_SCREEN_UPDATE_BPM_TIMER 10000 // 10 Seconds for demo purposes, will be changed later

// Sensor Task Timers
#define READ_SPO2_TIMER 60000
#define READ_TEMPERATURE_TIMER 60102

// Topbar
#define UPDATE_TOPBAR_TIMER 60000
#define UPDATE_TOPBAR_BATT_TIMER 60000

// RTC
#define RTC_UPDATE_TIMER 1000
#define RTC_CALCULATE_TIME 58001

// Sensor Data Logger
#define HEART_SENSOR_LOG_GENERIC_TIMER 5000
#define HEART_SENSOR_ESP_NOW_LOG_TIMER 10000
#define HEART_SENSOR_LOG_IR_TIMER 20

class settings {
public:
    // not like i'll use or sell it sooooooooo i'll just leave the idea here
    // void change_settings_port_read(void *pvParameters);
    // 8C:A4:08:80:60:AB
    // e0:e2:e6:63:0f:50
    // e0:e2:e6:63:0f:51
    //uint8_t server_mac_adress[6] = {0xE0, 0xE2, 0xE6, 0x63, 0x0F, 0x50}; // STA
    //uint8_t server_mac_adress[6] = {0x8C, 0xA4, 0x08, 0x80, 0x60, 0xAB}; // DEFAULT MAC
    uint8_t server_mac_adress[6] = {0xE0, 0xE2, 0xE6, 0x63, 0x0F, 0x51}; // AP
    char server_ip_address[32] = "";
    const char* PMK_KEY_STR = "U@Kw%nV&PzbRK2WM";
    const char* LMK_KEY_STR = "Ruth:WqH9@j%h5qg";
};