#include "ataos.h"

void heart_screen::draw_heart_screen(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    int last_screen_page = -1; // Track the last screen mode

    while (1) {
        // Wait for the semaphore to be signaled (indicating a screen change)
        if (xSemaphoreTake(ataos->xHeartScreenSemaphore, pdMS_TO_TICKS(50)) == pdTRUE) {
            // Check if the screen mode has changed
            LOG_DEBUG(HEART_SCREEN_LOG_TAG, "Heart Screen Active");

            ataos->clear_screen(); // Clear the screen area

            // Initialize the heart rate screen (draw static text)
            //ataos->watch_tft.setCursor(0, 30);
            //ataos->watch_tft.setTextSize(2);
            //ataos->smooth_print("Heart Rate");

            ataos->watch_tft.setCursor(0, 40);
            String bpm_avg = String(ataos->watch_heart_sensor.beat_avg) + "bpm";
            ataos->line_removal_print_middle(bpm_avg, 3);

            ataos->watch_tft.setCursor(0, 85);
            if (ataos->watch_heart_sensor.spo2_valid) {
                String spo2 = String(ataos->watch_heart_sensor.spo2) + " SpO2";
                ataos->line_removal_print_middle(spo2, 2);
            }

            ataos->watch_tft.setCursor(0, 110);
            String body_temp = String(ataos->watch_heart_sensor.temperature) + " C";
            ataos->line_removal_print_middle(body_temp, 2);
        }

        vTaskDelay(pdMS_TO_TICKS(WATCH_SCREEN_PRESS_RESPONSE_TIMER));
    }
}

void heart_screen::heart_screen_update_bpm(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // Ensure BPM updates happen regardless of the semaphore being given
        if (ataos->watch_screen.current_screen_page == SCREEN_HEARTRATE) {

            ataos->watch_tft.setCursor(0, 40);
            String bpm = String(ataos->watch_heart_sensor.beat) + "bpm";
            String bpm_avg = String(ataos->watch_heart_sensor.beat_avg) + "bpm";
            ataos->line_removal_print_middle(bpm_avg, 3);
            

            ataos->watch_tft.setCursor(0, 85);
            String spo2 = String(ataos->watch_heart_sensor.spo2) + " SpO2";
            if (ataos->watch_heart_sensor.spo2_valid) {
                ataos->line_removal_print_middle(spo2, 2);
            }

            ataos->watch_tft.setCursor(0, 110);
            String body_temp = String(ataos->watch_heart_sensor.temperature) + " C";
            ataos->line_removal_print_middle(body_temp, 2);

            
            LOG_DEBUG(HEART_SCREEN_LOG_TAG, "BPM: %s", bpm);
            LOG_DEBUG(HEART_SCREEN_LOG_TAG, "BPM_AVG: %s", bpm_avg);
            LOG_DEBUG(HEART_SCREEN_LOG_TAG, "SpO2: %s", spo2);
            LOG_DEBUG(HEART_SCREEN_LOG_TAG, "TEMP: %s", body_temp);
        }

        vTaskDelay(pdMS_TO_TICKS(HEART_SCREEN_UPDATE_BPM_TIMER));
    }
}