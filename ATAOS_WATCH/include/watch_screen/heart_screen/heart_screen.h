#pragma once

class heart_screen {
public:
    void draw_heart_screen(void *pvParameters);
    void heart_screen_update_bpm(void *pvParameters);

    bool is_heart_screen_active = false;
};