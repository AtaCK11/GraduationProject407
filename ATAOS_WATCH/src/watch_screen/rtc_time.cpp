#include <esp_now.h>
#include <WiFi.h>

#include "ataos.h"
#include "watch_screen/rtc_time.h"
#include "logger/logger.h"
#include "settings.h"

TimeInformation server_received_time;

void on_data_recv_time(const uint8_t* messageInfo, const uint8_t *data, int data_len) {
    // Check if the received data is the time data
    LOG_DEBUG(TIME_REQUEST_LOG_TAG, "ESP-NOW Data received from server! Length: %d", data_len);
    if (data_len == sizeof(TimeInformation)) {
        memcpy(&server_received_time, data, sizeof(TimeInformation));
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "ESP-NOW Data Written: %d", data_len);
    }
}

void rtc_time::request_time(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        // print the mac address of  ataos->watch_settings->server_mac_adress
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Server MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", ataos->watch_settings.server_mac_adress[0],  ataos->watch_settings.server_mac_adress[1],  ataos->watch_settings.server_mac_adress[2],  ataos->watch_settings.server_mac_adress[3],  ataos->watch_settings.server_mac_adress[4],  ataos->watch_settings.server_mac_adress[5]);

        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Enabling ESP-NOW...");

        if (WiFi.getMode() != WIFI_STA) {
            WiFi.mode(WIFI_STA);
        }
        vTaskDelay(pdMS_TO_TICKS(200)); //delay(100);
        if (esp_now_init() != ESP_OK) {
            LOG_ERROR(TIME_REQUEST_LOG_TAG, "ESP-NOW initialization failed!");
        }

        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr,  ataos->watch_settings.server_mac_adress, 6);
        peerInfo.channel = 1;
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            LOG_ERROR(TIME_REQUEST_LOG_TAG, "Failed to add ESP-NOW peer!");
        }

        esp_err_t result_recv = esp_now_register_recv_cb(on_data_recv_time);
        if (result_recv != ESP_OK) {
            LOG_ERROR(TIME_REQUEST_LOG_TAG, "Failed to register receive callback");
        }

        // Prepare time request
        strcpy(requested_time.requestType, "Time"); // Example request type

        // Send time request via ESP-NOW
        esp_err_t result = esp_now_send(ataos->watch_settings.server_mac_adress, (uint8_t *)&requested_time, sizeof(requested_time));
        if (result == ESP_OK) {
            LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Time request sent!");
        } else {
            LOG_ERROR(TIME_REQUEST_LOG_TAG, "Failed to send request! Error: 0x%X", result);
        }

        // Wait for response (timeout of 5 seconds)
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 seconds
        if (server_received_time.hour != NULL ||
            server_received_time.second != NULL) {
            // If response is received, store time data
            received_time = server_received_time;
            LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Time data received!");
        } else {
            LOG_ERROR(TIME_REQUEST_LOG_TAG, "No response received from server.");
        }

        // Go to sleep
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Disabling ESP-NOW...");
        esp_now_unregister_recv_cb();
        esp_now_deinit();
        //esp_now
        WiFi.mode(WIFI_OFF);


        vTaskDelay(pdMS_TO_TICKS(SERVER_TIME_REQUEST_TIMER)); // Sleep for 10 seconds before the next cycle --> will be around 5 hours or if its requested by the user
    }
}

void rtc_time::calculate_time(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "+1 Minute Time...");
        if (ataos->watch_time.received_time.minute == 59) {
            ataos->watch_time.received_time.minute = 0;
            if (ataos->watch_time.received_time.hour == 23) {
                ataos->watch_time.received_time.hour = 0;
            } else {
                ataos->watch_time.received_time.hour = ataos->watch_time.received_time.hour + 1;
            }
        } else {
            ataos->watch_time.received_time.minute = ataos->watch_time.received_time.minute + 1;
        }
        if (ataos->watch_screen.current_screen_page == SCREEN_HOME) {
            xSemaphoreGive(ataos->xHomeScreenSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(RTC_CALCULATE_TIME)); // Sleep for 1 minute
    }
}
