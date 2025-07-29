#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "ataos.h"
#include "bitmaps/weather_bitmaps.h" // TODO: remove this and add new images to topbar_bitmaps.h
#include "bitmaps/topbar_bitmaps.h"
#include "logger/logger.h"

/*

typedef struct {
    char city[32];
    char weather_description[32];
    float temperature;
    int humidity;
    float wind_speed;
} WeatherData;

*/

void topbar::update_topbar_weather(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // print the weather data
        LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Weather Data: %s, %s, %.2f, %d, %.2f", ataos->watch_weather.received_weather.city, ataos->watch_weather.received_weather.weather_description, ataos->watch_weather.received_weather.temperature, ataos->watch_weather.received_weather.humidity, ataos->watch_weather.received_weather.wind_speed);

        // update the topbar with the weather description and show the corresponding weather icon
        if (strcmp(ataos->watch_weather.received_weather.weather_description, "Gunesli") == 0) {
            // clear sky
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Clear Sky");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_clear_sky, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "few clouds") == 0) {
            // few clouds
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Few Clouds");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_few_clouds, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "scattered clouds") == 0) {
            // scattered clouds
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Scattered Clouds");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_few_clouds, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "broken clouds") == 0) {
            // broken clouds
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Broken Clouds");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_few_clouds, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "shower rain") == 0) {
            // shower rain
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Shower Rain");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_rain, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "rain") == 0) {
            // rain
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Rain");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_rain, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "thunderstorm") == 0) {
            // thunderstorm
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Thunderstorm");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_thunderstorm, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "snow") == 0) {
            // snow
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Snow");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_snow, 20, 20);
        } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "mist") == 0) {
            // mist
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Mist");
            ataos->watch_tft.drawRGBBitmap(5, 3, epd_bitmap_topbar_mist, 20, 20);
    
        } else {
            // unknown weather
            LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Unknown Weather");
            //ataos->watch_tft.drawRGBBitmap(1, 1, epd_bitmap_unknown_static, 31, 31);
        }

        ataos->watch_tft.setCursor(30, 9);
        ataos->watch_tft.setTextSize(1);
        String temp = String((int)ataos->watch_weather.received_weather.temperature) + "\"C          ";
        ataos->clear_smooth_print();
        ataos->smooth_print(temp);

        vTaskDelay(pdMS_TO_TICKS(UPDATE_TOPBAR_TIMER)); // 60 seconds
    }
}


void topbar::update_topbar_battery(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {

        if (ataos->watch_topbar.battery_level == BATT_FULL) {
            ataos->watch_tft.drawRGBBitmap(127, 3, epd_bitmap_battery_full, 28, 15);
        } else if (ataos->watch_topbar.battery_level == BATT_LOW) {
            ataos->watch_tft.drawRGBBitmap(127, 3, epd_bitmap_battery_low, 28, 15);
        } else if (ataos->watch_topbar.battery_level == BATT_MED) {
            ataos->watch_tft.drawRGBBitmap(127, 3, epd_bitmap_battery_medium, 28, 15);
        } else if (ataos->watch_topbar.battery_level == BATT_CHARG) {
            ataos->watch_tft.drawRGBBitmap(127, 3, epd_bitmap_battery_charging, 28, 15);
        }

        vTaskDelay(pdMS_TO_TICKS(UPDATE_TOPBAR_BATT_TIMER)); // 60 seconds
    }
}