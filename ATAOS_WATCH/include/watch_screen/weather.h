#pragma once

#include "logger/logger.h"

typedef struct {
    char city[32];
    char weather_description[32];
    float temperature;
    int humidity;
    float wind_speed;
} WeatherData;

typedef struct {
    char requestType[16];  // Example: "Current", "Weekly"
} WeatherRequest;

/*
clear sky: açık gökyüzü
few clouds: az bulutlu
scattered clouds: dağınık bulutlu
broken clouds: parçalı bulutlu
shower rain: sağanak yağmur
rain: yağmur
thunderstorm: gök gürültülü fırtına
snow: kar
mist: sis
*/

class weather {
public:
    void request_weather(void *pvParameters);
    
    WeatherData received_weather;
    WeatherRequest requested_weather;

    // write a default constructor for received_weather and requested_weather
    weather() {
        strcpy(received_weather.city, "Ankara");
        strcpy(received_weather.weather_description, "Gunesli");
        received_weather.temperature = 28.4;
        received_weather.humidity = 29;
        received_weather.wind_speed = 16;

        strcpy(requested_weather.requestType, "Current");
    }


private:
    bool responseReceived = false;

};
