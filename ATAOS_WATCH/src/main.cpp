#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "WiFi.h"

#include "bitmaps/cankaya_logo_bitmap.h"
#include "bitmaps/loading_bitmaps.h"
#include "bitmaps/weather_bitmaps.h"
#include "watch_screen/tft_screen.h"
#include "watch_screen/heart_screen/heart_screen.h"
#include "watch_screen/weather.h"
#include "watch_screen/topbar/topbar.h"
#include "settings.h"
#include "logger/logger.h"

#include "ataos.h"

#define PROD
#define DEV

ataos_firmware ataos;

// put function declarations here
void update_tft_screen(void * pvParameters);
void handle_button_press(void * pvParameters);
void who_am_i(void * pvParameters);
void ataos_debug(void * pvParameters);
void check_battery(void * pvParameters);

void setup() {
    Serial.begin(115200);
    LOG_DEBUG(SETUP_LOG_TAG, "- ATAOS v1-in-DEV");
    LOG_DEBUG(SETUP_LOG_TAG, "configTICK_RATE_HZ = %d", configTICK_RATE_HZ);
    
    delay(500);

    Wire.begin(27, 26); // SDA, SCL
    delay(500);

    Serial.print("ATAOS_WATCH MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Initialize SPI TFT
    ataos.watch_tft.initR(INITR_BLACKTAB);
    ataos.watch_tft.setRotation(1);

    ataos.watch_tft.fillScreen(ST7735_BLACK);
    //ataos.watch_tft.drawBitmap(20, 4, epd_bitmap_cankaya_logo, 120, 120, TFT_YELLOW_COLOR);

    ataos.xWeatherScreenSemaphore = xSemaphoreCreateBinary();
    ataos.xHomeScreenSemaphore = xSemaphoreCreateBinary();
    ataos.xHeartScreenSemaphore = xSemaphoreCreateBinary();
    ataos.xUpdateTimeSemaphore = xSemaphoreCreateBinary();

    delay(2500);
    pinMode(BUTTON_PIN_LEFT, INPUT_PULLUP);
    pinMode(BUTTON_PIN_HOME, INPUT_PULLUP);
    pinMode(BUTTON_PIN_RIGHT, INPUT_PULLUP);
    pinMode(ataos.BATTERY_PIN, INPUT_PULLUP);
    delay(500);

    if (ataos.watch_heart_sensor.particleSensor.begin(Wire, I2C_SPEED_FAST) == false) {
        LOG_ERROR(HEART_SENSOR_LOG_TAG, "MAX30105 was not found. Please check wiring/power. ");
        return;
    }
    delay(500);

    ataos.watch_heart_sensor.particleSensor.setup();
    ataos.watch_heart_sensor.particleSensor.setPulseAmplitudeRed(0x7F); // 0x33 = 10mA
    ataos.watch_heart_sensor.particleSensor.setPulseAmplitudeGreen(0);

    //ataos.watch_heart_sensor.particleSensor.setPulseAmplitudeIR(0); // 0x33 = 10mA

    ataos.watch_tft.fillScreen(ST7735_BLACK);
    for (int i = 0; i < 9; i++) {
        //ataos.watch_tft.fillScreen(ST7735_BLACK);
        if (i == 0) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame1, 100, 100);
        } else if (i == 1) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame2, 100, 100);
        } else if (i == 2) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame3, 100, 100);
        } else if (i == 3) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame4, 100, 100);
        } else if (i == 4) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame1, 100, 100);
        } else if (i == 5) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame2, 100, 100);
        } else if (i == 6) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame3, 100, 100);
        } else if (i == 7) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame4, 100, 100);
        } else if (i == 8) {
            ataos.watch_tft.drawRGBBitmap(30, 10, epd_bitmap_loading_frame1, 100, 100);
        }
        delay(300);
    }

    esp_sleep_enable_ext1_wakeup((1ULL << BUTTON_PIN_LEFT) | (1ULL << BUTTON_PIN_HOME) | (1ULL << BUTTON_PIN_RIGHT), ESP_EXT1_WAKEUP_ANY_HIGH);

    delay(3000);

    LOG_DEBUG(SETUP_LOG_TAG, "Setup Complete... Starting Loop");

    ataos.watch_tft.fillScreen(ST7735_BLACK);
    ataos.watch_tft.setTextSize(2);
    ataos.watch_tft.setCursor(10, 30);
    ataos.watch_tft.println("PRESS HOME     TO START");

    xTaskCreate(update_tft_screen, "Update TFT Screen", 4098, NULL, 3, NULL);
    xTaskCreate(handle_button_press, "Handle Button Press", 4098, NULL, 3, NULL);


    delay(1000);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_time.request_time(pvParameters);
    }, "Request Time", 4098, &ataos, 3, NULL);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_topbar.update_topbar_weather(pvParameters);
    }, "Upd Topbar Weather", 4098, &ataos, 2, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_weather_screen.draw_weather_screen(pvParameters);
    }, "Weather Screen", 4098, &ataos, 2, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_home_screen.draw_home_screen(pvParameters);
    }, "Home Screen", 4098, &ataos, 2, NULL);

    
    delay(100);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_time.calculate_time(pvParameters);
    }, "RTC Timer", 4098, &ataos, 1, NULL);

    delay(100);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.run_heart_sensor(pvParameters);
    }, "Heart Sensor Tasks", 8192, &ataos, 2, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.read_temperature(pvParameters);
    }, "Heart Sensor Temp", 4098, &ataos, 2, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.read_spo2(pvParameters);
    }, "Heart Sensor SpO2", 4098, &ataos, 2, NULL);

    delay(500);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.log_sensor_data(pvParameters);
    }, "Heart Sensor Log", 4098, &ataos, 2, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.log_ir_data(pvParameters);
    }, "Heart Sensor Log IR", 4098, &ataos, 2, NULL);

    
    delay(500);

    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_screen.draw_heart_screen(pvParameters);
    }, "Heart Screen", 4098, &ataos, 2, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_screen.heart_screen_update_bpm(pvParameters);
    }, "Heart Screen BPM", 4098, &ataos, 2, NULL);

    // who_am_i task
    xTaskCreate(who_am_i, "Who Am I", 4098, NULL, 1, NULL);
    //xTaskCreate(ataos_debug, "ATAOS Debug", 4098, NULL, 1, NULL);

    //xTaskCreate([](void * pvParameters) {
    //    ataos.send_data_to_server(pvParameters);
    //}, "Send Data to Server", 8192, &ataos, 2, NULL);

    xTaskCreate(check_battery, "Check Battery", 4098, NULL, 1, NULL);
    xTaskCreate([](void * pvParameters) {
        ataos.watch_topbar.update_topbar_battery(pvParameters);
    }, "Upd Battery", 4098, &ataos, 1, NULL);

    vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreate([](void * pvParameters) {
        ataos.watch_heart_sensor.esp_now_send_hr(pvParameters);
    }, "Heart Sensor ESP_NOW", 8192, &ataos, 3, NULL);

    LOG_DEBUG(SETUP_LOG_TAG, "Starting Scheduler");
    //vTaskStartScheduler();
}

void loop() { /* boooo! */ }

void update_tft_screen(void * pvParameters) {
    while (1) {
        String screen_text = "ERR";
        bool screen_needs_update = false;


        switch (ataos.watch_screen.button_state) {
        case BUTTON_LEFT_PRESSED:
            screen_text = "Left Button Pressed - Weather Menu";
            ataos.watch_screen.wannabe_screen_page = SCREEN_WEATHER;
            ataos.clear_screen(); // Clear the screen area
            xSemaphoreGive(ataos.xWeatherScreenSemaphore);
            break;

        case BUTTON_HOME_PRESSED:
            screen_text = "Home Button Pressed - Home Screen";
            ataos.watch_screen.wannabe_screen_page = SCREEN_HOME;
            ataos.clear_screen(); // Clear the screen area
            xSemaphoreGive(ataos.xHomeScreenSemaphore);
            break;

        case BUTTON_RIGHT_PRESSED:
            screen_text = "Right Button Pressed - Heart Rate Menu";
            ataos.watch_screen.wannabe_screen_page = SCREEN_HEARTRATE;
            ataos.clear_screen(); // Clear the screen area
            xSemaphoreGive(ataos.xHeartScreenSemaphore);
            break;

        case BUTTON_LEFT_DOUBLE_CLICK:
            screen_text = "Left Button Double Clicked - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            //ataos.watch_tft.drawRGBBitmap(1, 1, epd_bitmap_rain_static, 31, 31);
            break;

        case BUTTON_HOME_DOUBLE_CLICK:
            screen_text = "Home Button Double Clicked - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            //ataos.watch_tft.drawRGBBitmap(1, 1, epd_bitmap_snow_static, 31, 31);
            break;

        case BUTTON_RIGHT_DOUBLE_CLICK:
            screen_text = "Right Button Double Clicked - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
           // ataos.watch_tft.drawRGBBitmap(1, 1, epd_bitmap_clear_sky_static, 31, 31);
            break;

        case BUTTON_LEFT_HOLDING:
            screen_text = "Left Button Holding - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            analogWrite(TFT_BLK, 255);
            break;

        case BUTTON_HOME_HOLDING:
            screen_text = "Home Button Holding - Settings";
            ataos.watch_screen.wannabe_screen_page = SCREEN_SETTINGS;
            break;

        case BUTTON_RIGHT_HOLDING:
            screen_text = "Right Button Holding - No Idea";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            analogWrite(TFT_BLK, 0);
            break;

        case BUTTON_NONE:
        default:
            screen_text = "No Button Pressed - Dont change the screen";
            ataos.watch_screen.wannabe_screen_page = ataos.watch_screen.current_screen_page;
            break;
        }

        if (ataos.watch_screen.current_screen_page != ataos.watch_screen.wannabe_screen_page) {
            screen_needs_update = true;
        }

        if (screen_needs_update) {
            //ataos.watch_tft.fillRect(0, 30, 160, 140, ST7735_BLACK); // Clear the screen area
            //ataos.watch_tft.setTextColor(TFT_WHITE_COLOR);
            //ataos.watch_tft.setTextSize(2);
            //ataos.watch_tft.setCursor(0, 30);
            //ataos.watch_tft.println(screen_text);
            ataos.watch_screen.current_screen_page = ataos.watch_screen.wannabe_screen_page;
            LOG_DEBUG(SCREEN_LOG_TAG, "Screen Updated: %s", screen_text.c_str());
        }

        vTaskDelay(pdMS_TO_TICKS(WATCH_SCREEN_REFRESH_TIMER));
    }
}

void handle_button_press(void *pvParameters) {
    static TickType_t last_debounce_time = 0;
    static TickType_t last_press_time = 0;
    static ButtonState last_button_pressed = BUTTON_NONE;
    static bool waiting_for_double_click = false;

    while (1) {
        ButtonState current_button_state = BUTTON_NONE;

        // cooldown
        if (ataos.watch_screen.button_in_cooldown) {
            if ((xTaskGetTickCount() - last_press_time) >= pdMS_TO_TICKS(BUTTON_COOLDOWN_TIME)) {
                ataos.watch_screen.button_in_cooldown = false;
            }
        }

        if (!ataos.watch_screen.button_in_cooldown) {
            if (digitalRead(BUTTON_PIN_LEFT) == HIGH) {
                current_button_state = BUTTON_LEFT_PRESSED;
            } else if (digitalRead(BUTTON_PIN_HOME) == HIGH) {
                current_button_state = BUTTON_HOME_PRESSED;
            } else if (digitalRead(BUTTON_PIN_RIGHT) == HIGH) {
                current_button_state = BUTTON_RIGHT_PRESSED;
            }

            // debounce
            if (current_button_state != ataos.watch_screen.last_button_state) {
                last_debounce_time = xTaskGetTickCount();
            }

            // after the debounce delay, check if the state has really changed.
            if ((xTaskGetTickCount() - last_debounce_time) > pdMS_TO_TICKS(BUTTON_DEBOUNCE_DELAY)) {
                if (current_button_state != ataos.watch_screen.button_state) {
                    ataos.watch_screen.button_state = current_button_state;

                    if (ataos.watch_screen.button_state != BUTTON_NONE) {
                        // Check for a double click: same button pressed within the double click interval.
                        if (ataos.watch_screen.button_state == last_button_pressed &&
                            ((xTaskGetTickCount() - last_press_time) < pdMS_TO_TICKS(BUTTON_DOUBLE_CLICK_TIME))) {
                            // double click
                            switch (ataos.watch_screen.button_state) {
                                case BUTTON_LEFT_PRESSED:
                                    ataos.watch_screen.button_state = BUTTON_LEFT_DOUBLE_CLICK;
                                    LOG_DEBUG(BUTTON_LOG_TAG, "Left Button Double Clicked");
                                    break;
                                case BUTTON_HOME_PRESSED:
                                    ataos.watch_screen.button_state = BUTTON_HOME_DOUBLE_CLICK;
                                    LOG_DEBUG(BUTTON_LOG_TAG, "Home Button Double Clicked");
                                    break;
                                case BUTTON_RIGHT_PRESSED:
                                    ataos.watch_screen.button_state = BUTTON_RIGHT_DOUBLE_CLICK;
                                    LOG_DEBUG(BUTTON_LOG_TAG, "Right Button Double Clicked");
                                    break;
                                default:
                                    break;
                            }
                            waiting_for_double_click = false;
                            // Set cooldown
                            ataos.watch_screen.button_in_cooldown = true;
                            last_press_time = xTaskGetTickCount();
                        } else {
                            // Single click detected, but wait to see if a double click follows
                            waiting_for_double_click = true;
                            last_press_time = xTaskGetTickCount();
                            LOG_DEBUG(BUTTON_LOG_TAG, "Single Click Detected");
                        }
                        last_button_pressed = ataos.watch_screen.button_state;
                    }
                }
            }
        }

        // Check for a button hold.
        if (ataos.watch_screen.button_state != BUTTON_NONE && digitalRead(last_button_pressed) == HIGH) {
            if ((xTaskGetTickCount() - last_press_time) > pdMS_TO_TICKS(BUTTON_HOLD_TIME)) {
                switch (last_button_pressed) {
                    case BUTTON_LEFT_PRESSED:
                        ataos.watch_screen.button_state = BUTTON_LEFT_HOLDING;
                        LOG_DEBUG(BUTTON_LOG_TAG, "Left Button Holding");
                        break;
                    case BUTTON_HOME_PRESSED:
                        ataos.watch_screen.button_state = BUTTON_HOME_HOLDING;
                        LOG_DEBUG(BUTTON_LOG_TAG, "Home Button Holding");
                        break;
                    case BUTTON_RIGHT_PRESSED:
                        ataos.watch_screen.button_state = BUTTON_RIGHT_HOLDING;
                        LOG_DEBUG(BUTTON_LOG_TAG, "Right Button Holding");
                        break;
                    default:
                        break;
                }
                ataos.watch_screen.button_in_cooldown = true;
                last_press_time = xTaskGetTickCount();
            }
        }

        if (waiting_for_double_click &&
            ((xTaskGetTickCount() - last_press_time) > pdMS_TO_TICKS(BUTTON_DOUBLE_CLICK_TIME))) {
            waiting_for_double_click = false;
            LOG_DEBUG("BUTTON", "Double Click Timeout");
        }

        ataos.watch_screen.last_button_state = current_button_state;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void who_am_i(void * pvParameters) {
    while (1) {
        if (Serial.available() > 0) {
            String command = Serial.readStringUntil('\n');
            if (command == "whoami") {
                String ss = "ATAOS_SMARTWATCH";
                char aa2[ss.length() + 1];
                ss.toCharArray(aa2, ss.length() + 1);
                LOG_DEBUG(WHO_AM_I_LOG_TAG, "%s", aa2);
            }   else if (command == "whatsmymac") {
                String ss = WiFi.macAddress();
                // convert into char array
                char mac_char_array[ss.length() + 1];
                ss.toCharArray(mac_char_array, ss.length() + 1);
                LOG_DEBUG(WHO_AM_I_LOG_TAG, "%s", mac_char_array);
            } else {
                LOG_DEBUG(WHO_AM_I_LOG_TAG, "Unknown Command");
            }

        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void check_battery(void * pvParameters) {
    while (1) {
        int raw = analogRead(ataos.BATTERY_PIN);
        // The battery can maximum output 2.1V to the pin from voltage divider.
        // The ESP32 can read voltages 2.45V max.
        
        if (ataos.is_charging == 3) {
            LOG_DEBUG(BATTERY_LOG_TAG, "Battery is charging");
            ataos.watch_topbar.battery_level = BATT_CHARG;
            analogWrite(TFT_BLK, 0);
            vTaskDelay(pdMS_TO_TICKS(50));
            esp_light_sleep_start();
            vTaskDelay(pdMS_TO_TICKS(50));
            analogWrite(TFT_BLK, 4095);
            ataos.is_charging = 0;
        } else if (raw == 4095) {
            LOG_DEBUG(BATTERY_LOG_TAG, "Battery is charging");
            ataos.watch_topbar.battery_level = BATT_CHARG;
            ataos.is_charging++;
        } else if (raw > 3400) {
            LOG_DEBUG(BATTERY_LOG_TAG, "Battery is full");
            ataos.watch_topbar.battery_level = BATT_FULL;
            ataos.is_charging = 0;
        } else if (raw < 2500 && raw > 1500) {
            LOG_DEBUG(BATTERY_LOG_TAG, "Battery is LOW");
            ataos.watch_topbar.battery_level = BATT_LOW;
            ataos.is_charging = 0;
        } else if (raw > 2500 && raw < 3500) {
            LOG_DEBUG(BATTERY_LOG_TAG, "Battery is medium");
            ataos.watch_topbar.battery_level = BATT_MED;
            ataos.is_charging = 0;
        } 
        
        LOG_DEBUG(BATTERY_LOG_TAG, "Raw read: %d", raw);

        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}