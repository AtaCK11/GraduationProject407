#include "ataos.h"
#include "bitmaps/weather_bitmaps.h"

void weather_screen::draw_weather_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // Wait for the semaphore to be signaled (indicating a screen change)
        if (xSemaphoreTake(ataos->xWeatherScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {

            ataos->clear_screen(); // Clear the screen area

            // log the data that will be displayed on the screen
            LOG_DEBUG(WEATHER_SCREEN_LOG_TAG, "City: %s", ataos->watch_weather.received_weather.city);
            LOG_DEBUG(WEATHER_SCREEN_LOG_TAG, "Weather: %s", ataos->watch_weather.received_weather.weather_description);
            LOG_DEBUG(WEATHER_SCREEN_LOG_TAG, "Temperature: %.2f", ataos->watch_weather.received_weather.temperature);
            LOG_DEBUG(WEATHER_SCREEN_LOG_TAG, "Humidity: %d", ataos->watch_weather.received_weather.humidity);
            LOG_DEBUG(WEATHER_SCREEN_LOG_TAG, "Wind Speed: %.2f", ataos->watch_weather.received_weather.wind_speed);

            if (strcmp(ataos->watch_weather.received_weather.weather_description, "Gunesli") == 0) {
                // clear sky
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_clear_sky_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "few clouds") == 0) {
                // few clouds
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_few_clouds_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "scattered clouds") == 0) {
                // scattered clouds
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_few_clouds_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "broken clouds") == 0) {
                // broken clouds
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_few_clouds_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "shower rain") == 0) {
                // shower rain
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_rain_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "rain") == 0) {
                // rain
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_rain_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "thunderstorm") == 0) {
                // thunderstorm
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_thunderstorm_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "snow") == 0) {
                // snow
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_snow_static, 31, 31);
            } else if (strcmp(ataos->watch_weather.received_weather.weather_description, "mist") == 0) {
                // mist
                ataos->watch_tft.drawRGBBitmap(64, 30, epd_bitmap_mist_static, 31, 31);
        
            } else {
                // unknown weather
                LOG_DEBUG(TOPBAR_WEATHER_LOG_TAG, "Unknown Weather");
                //ataos->watch_tft.drawRGBBitmap(35, 35, epd_bitmap_unknown_static, 31, 31);
            }

            // weather
            String current_weather = ataos->watch_weather.received_weather.weather_description;
            ataos->watch_tft.setCursor(0, 65);
            ataos->clear_smooth_print();
            ataos->smooth_print_middle(current_weather, 2);
            
            // temp
            String current_temperature = String(ataos->watch_weather.received_weather.temperature) + " C";
            ataos->watch_tft.setCursor(0, 85);
            ataos->clear_smooth_print();
            ataos->smooth_print_middle(current_temperature, 2);

            // humidity
            String current_humidity = "Nem: " + String(ataos->watch_weather.received_weather.humidity) + "%";
            ataos->watch_tft.setCursor(0, 105);
            ataos->clear_smooth_print();
            ataos->smooth_print_middle(current_humidity, 2);


            // wind?
            
        }

        vTaskDelay(pdMS_TO_TICKS(WATCH_SCREEN_PRESS_RESPONSE_TIMER));
    }
}
