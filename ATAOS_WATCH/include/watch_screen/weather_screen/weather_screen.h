#pragma once

#include "logger/logger.h"

class weather_screen {
public:
    void draw_weather_screen(void *pvParameters);

    bool is_weather_screen_active = false;
};