#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "WiFi.h"

#include "logger/logger.h"


#include "watch_screen/heart_screen/heart_sensor.h"

#include "watch_screen/screen.h"
#include "watch_screen/weather_screen/weather_screen.h"
#include "watch_screen/home_screen/home_screen.h"
#include "watch_screen/heart_screen/heart_screen.h"
#include "watch_screen/topbar/topbar.h"

#include "watch_screen/weather.h"
#include "watch_screen/rtc_time.h"

#include "settings.h"


class ataos_firmware {
public:
    // Screen drawing functions

    void smooth_print(String text);
    void smooth_print(int value);
    void smooth_print(float value);
    void smooth_print_middle(String text, int text_size);
    void smooth_print_middle(int value, int text_size);
    void smooth_print_middle(float value, int text_size);

    void line_removal_print(String text, int text_size);
    void line_removal_print(int value, int text_size);
    void line_removal_print(float value, int text_size);
    void line_removal_print_middle(String text, int text_size);
    void line_removal_print_middle(int value, int text_size);
    void line_removal_print_middle(float value, int text_size);
    
    void agressive_print(String text);
    void clear_smooth_print();
    void clear_screen();
    void clear_entire_screen();

    void send_data_to_server(void *pvParameters);

    bool register_device_with_server();
    bool get_pc_ip_from_ap();

    bool first_start = true;
    const int BATTERY_PIN = 35;
    const float R1 = 100000;
    const float R2 = 100000;
    const float MAX_VOLTAGE = 4.2;

    String DEVICETYPE = "WearableDevice";

    int is_charging = 0;

    Adafruit_ST7735 watch_tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

    settings watch_settings;
    screen watch_screen;
    weather watch_weather;
    rtc_time watch_time;
    topbar watch_topbar;

    heart_sensor watch_heart_sensor;

    heart_screen watch_heart_screen;
    weather_screen watch_weather_screen;
    home_screen watch_home_screen;

    // semaphores
    SemaphoreHandle_t xWeatherScreenSemaphore = NULL;
    SemaphoreHandle_t xHomeScreenSemaphore = NULL;
    SemaphoreHandle_t xHeartScreenSemaphore = NULL;
    SemaphoreHandle_t xUpdateTimeSemaphore = NULL;


};