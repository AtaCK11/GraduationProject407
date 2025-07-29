#include "ataos.h"

void home_screen::draw_home_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // Wait for the semaphore to be signaled (indicating a screen change)
        if (xSemaphoreTake(ataos->xHomeScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
            LOG_DEBUG(HOME_SCREEN_LOG_TAG, "Home Screen Active");

            // log the data that will be displayed on the screen
            LOG_DEBUG(HOME_SCREEN_LOG_TAG, "Date: %s", ataos->watch_time.received_time.date);
            LOG_DEBUG(HOME_SCREEN_LOG_TAG, "Day: %s", ataos->watch_time.received_time.day_name);
            LOG_DEBUG(HOME_SCREEN_LOG_TAG, "Time: %d:%d", ataos->watch_time.received_time.hour, ataos->watch_time.received_time.minute);
            
            ataos->clear_smooth_print();
            // Draw the current date
            String current_date = ataos->watch_time.received_time.date;
            ataos->watch_tft.setCursor(20, 110);
            ataos->clear_smooth_print();
            ataos->smooth_print_middle(current_date, 1);
            
            // change ataos->watch_time.received_time.day_name to string
            String day_name = ataos->watch_time.received_time.day_name;
            ataos->watch_tft.setCursor(30, 90);
            ataos->clear_smooth_print();
            ataos->smooth_print_middle(day_name, 2);

            // Draw the current time
            // write like 01:06
            String current_time = String(ataos->watch_time.received_time.hour) + ":" + String(ataos->watch_time.received_time.minute);
            if (ataos->watch_time.received_time.hour < 10) {
                current_time = "0" + current_time;
            }
            if (ataos->watch_time.received_time.minute < 10) {
                current_time = current_time.substring(0, 3) + "0" + current_time.substring(3);
            }

            ataos->watch_tft.setCursor(20, 40); // 160x128
            //ataos->watch_tft.setTextSize(4); // 4:24x32 , 5: 30x40
            ataos->clear_smooth_print();
            ataos->smooth_print_middle(current_time, 4);

        }

        // Ensure the screen updates happen regardless of the semaphore being given
        if (ataos->watch_screen.current_screen_page == SCREEN_HOME) {
            if (xSemaphoreTake(ataos->xHomeScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
                // Draw the current time
                // write like 01:06
                String current_time = String(ataos->watch_time.received_time.hour) + ":" + String(ataos->watch_time.received_time.minute);
                if (ataos->watch_time.received_time.hour < 10) {
                    current_time = "0" + current_time;
                }
                if (ataos->watch_time.received_time.minute < 10) {
                    current_time = current_time.substring(0, 3) + "0" + current_time.substring(3);
                }

                ataos->watch_tft.setCursor(20, 40); // 160x128
                //ataos->watch_tft.setTextSize(4); // 4:24x32 , 5: 30x40
                ataos->clear_smooth_print();
                ataos->smooth_print_middle(current_time, 4);
            }
        }

        // Small delay to prevent hogging the CPU
        vTaskDelay(pdMS_TO_TICKS(WATCH_SCREEN_PRESS_RESPONSE_TIMER));
    }
}
