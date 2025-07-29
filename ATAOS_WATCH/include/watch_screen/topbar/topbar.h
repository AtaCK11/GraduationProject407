#pragma once

#define BATT_FULL   0
#define BATT_LOW    1 
#define BATT_MED    2
#define BATT_CHARG  3

// weather, battery
class topbar{
public:
    void update_topbar_weather(void *pvParameters);
    void update_topbar_battery(void *pvParameters);

    int battery_level = 0;
};
