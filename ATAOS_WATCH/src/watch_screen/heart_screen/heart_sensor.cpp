#include <Arduino.h>
//#include <spo2_algorithm.h>
#include "ataos.h"

#include <esp_now.h>
#include <WiFi.h>

// data for heart sensor esp-now
#define HEART_SENSOR_ESPNOW_TAG 1
#define HEART_SENSOR_TEMPERATURE_ESPNOW_TAG 2
#define HEART_SENSOR_SPO2_ESPNOW_TAG 3

typedef struct {
    int heart_rate;
    float temperature;
    int spo2;
} HRInformation;


void heart_sensor::run_heart_sensor(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    TickType_t last_beat_tick = xTaskGetTickCount();
    while (1) {
        //LOG_DEBUG(HEART_SENSOR_LOG_TAG, "Found MAX30105");
        //LOG_DEBUG(HEART_SENSOR_LOG_TAG, "%d", configTICK_RATE_HZ);
        long irValue = particleSensor.getIR();

        if (irValue < 50000) {
            //LOG_INFO(HEART_SENSOR_LOG_TAG, "Finger not detected");
        } else {
            ir_particle = irValue;
            sample_hr_plot[sample_hr_plot_spot++] = irValue;
            sample_hr_plot_spot %= 250; // dürüm
        }

        
        if (checkForBeat(irValue)) {
            TickType_t current_tick = xTaskGetTickCount();
            TickType_t delta_ticks = current_tick - last_beat_tick;
            last_beat_tick = current_tick;

            
            red_particle = particleSensor.getRed();

            float deltaSeconds = (delta_ticks * portTICK_PERIOD_MS) / 1000.0;

            // Calculate beats per minute (BPM)
            float bpm = 60.0 / deltaSeconds;

            // Only process valid BPM values.
            if (bpm > 20 && bpm < 255) {
                // Store the BPM (cast to a byte) in the rates array
                rates[rate_spot++] = (byte)bpm;
                rate_spot %= RATE_SIZE; // Wrap the index if needed

                // Compute the average BPM from the stored rates
                beat_avg = 0;
                for (byte x = 0; x < RATE_SIZE; x++) {
                    beat_avg += rates[x];
                }
                beat_avg /= RATE_SIZE;
            }
        } 
        
        
        //LOG_INFO(HEART_SENSOR_LOG_TAG, "IR=%d, BPM=%d, Avg BPM=%d", irValue, mbpm, beat_avg);
        vTaskDelay(pdMS_TO_TICKS(HEART_SENSOR_TIMER));
        
    }
}

void heart_sensor::read_temperature(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        particleSensor.enableDIETEMPRDY();
        vTaskDelay(pdMS_TO_TICKS(500));
        temperature = particleSensor.readTemperature();
        LOG_INFO(HEART_SENSOR_TEMPERATURE_LOG_TAG, "%.2f", temperature);
        particleSensor.disableDIETEMPRDY();
        vTaskDelay(pdMS_TO_TICKS(READ_TEMPERATURE_TIMER));
        
    }
}

void heart_sensor::read_spo2(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        
        ataos->watch_heart_sensor.particleSensor.setPulseAmplitudeRed(0x33); // 0x33 = 10mA
        vTaskDelay(pdMS_TO_TICKS(100));

        for (int i = 0; i < 100; i++) {
            ir_buffer[i] = particleSensor.getIR();
            red_buffer[i] = particleSensor.getRed();
        }

        maxim_heart_rate_and_oxygen_saturation(ir_buffer, buffer_size, red_buffer, &spo2, &spo2_valid, &beat, &beat_valid);

        LOG_INFO(HEART_SENSOR_SPO2_LOG_TAG, "%d", spo2);

        ataos->watch_heart_sensor.particleSensor.setPulseAmplitudeRed(0);
        vTaskDelay(pdMS_TO_TICKS(READ_SPO2_TIMER));
    }
}

void heart_sensor::log_sensor_data(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {

        LOG_INFO(HEART_SENSOR_HR_LOG_TAG, "%d", beat_avg);
        LOG_INFO(HEART_SENSOR_TEMPERATURE_LOG_TAG, "%.2f", temperature);



        vTaskDelay(pdMS_TO_TICKS(HEART_SENSOR_LOG_GENERIC_TIMER));
    }
}

void heart_sensor::esp_now_send_hr(void *pvParameters) {
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

        // send data
        HRInformation hr_info;
        hr_info.heart_rate = beat_avg;
        hr_info.temperature = temperature;
        hr_info.spo2 = spo2;
        esp_err_t result = esp_now_send(ataos->watch_settings.server_mac_adress, (uint8_t *)&hr_info, sizeof(hr_info));
        if (result == ESP_OK) {
            LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Heart rate data sent!");
        } else {
            LOG_ERROR(TIME_REQUEST_LOG_TAG, "Failed to send heart rate data! Error: 0x%X", result);
        }

        // Go to sleep
        LOG_DEBUG(TIME_REQUEST_LOG_TAG, "Disabling ESP-NOW...");
        esp_now_unregister_recv_cb();
        esp_now_deinit();
        //esp_now
        WiFi.mode(WIFI_OFF);


        vTaskDelay(pdMS_TO_TICKS(HEART_SENSOR_ESP_NOW_LOG_TIMER)); // Sleep for 10 seconds before the next cycle --> will be around 5 hours or if its requested by the user
    }
}


void heart_sensor::log_ir_data(void *pvParameters) {
    ataos_firmware *ataos = (struct ataos_firmware *)pvParameters;
    while (1) {
        LOG_INFO(HEART_SENSOR_IR_LOG_TAG, "%d", ataos->watch_heart_sensor.ir_particle);
        vTaskDelay(pdMS_TO_TICKS(50000));
    }
}