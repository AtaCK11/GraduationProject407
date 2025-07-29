#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include <cJSON.h>
#include "esp_vfs_dev.h"
#include "driver/uart.h"

static const char *TAG = "ESP32_DB_SERVER";
static const char *SENSOR_TAG = "SENSOR_VALUE";

#define WIFI_SSID "BITIRME_AP_SERVER"
#define WIFI_PASS "12345678" // 07!1KpSg|:QY^3:x.um$0b$D

// Smartwatch keys // A0:DD:6C:72:51:40
uint8_t server_mac_adress[6] = {0xA0, 0xDD, 0x6C, 0x72, 0x51, 0x40};
const char* PMK_KEY_STR = "U@Kw%nV&PzbRK2WM";
const char* LMK_KEY_STR = "Ruth:WqH9@j%h5qg";

typedef struct {
    char day_name[32];
    char date[32];
    int hour;
    int minute;
    int second;
} TimeInformation;

typedef struct {
    char requestType[5];  // Example: "Time"
} TimeRequest;

typedef struct {
    int heart_rate;
    float temperature;
    int spo2;
} HRInformation;

// main IP from write_pc_db_server_handler
char *main_ip = NULL;

typedef struct {
    char mac[18];
    char ip[16];
} client_info_t;

client_info_t clients[5];
int client_count = 0;

// ---------------------- HTTP Handler ------------------------
// HTTP handler for /write_pc_db_server POST
esp_err_t write_pc_db_server_handler(httpd_req_t *req) {
    char buf[200];
    int ret = httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf)));
    if (ret <= 0) {
        ESP_LOGE(TAG, "Failed to receive data from client");
        return ESP_FAIL;
    }
    buf[ret] = '\0';  // Null-terminate the buffer

    ESP_LOGI(TAG, "Received data: %s", buf);

    // Search for the 'pc_ip' field in the JSON string
    char *ip_start = strstr(buf, "\"pc_ip\"");
    if (ip_start) {
        ip_start = strchr(ip_start, ':');  // find the colon
        if (ip_start) {
            ip_start++; // move past the colon
            while (*ip_start == ' ' || *ip_start == '\"') ip_start++; // skip whitespace and opening quote

            char *ip_end = strchr(ip_start, '\"');
            if (ip_end) {
                *ip_end = '\0';

                if (main_ip) free(main_ip);
                main_ip = strdup(ip_start);
                ESP_LOGI(TAG, "Main IP: %s", main_ip);
                httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
                return ESP_OK;
            }
        }
    }
    ESP_LOGE(TAG, "'pc_ip' not found or malformed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
}

// HTTP handler for /get_pc_ip GET
esp_err_t get_pc_ip_handler(httpd_req_t *req) {
    if (main_ip != NULL && strlen(main_ip) > 0) {
        httpd_resp_send(req, main_ip, HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    } else {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "PC IP not set yet");
        return ESP_FAIL;
    }
    return ESP_FAIL;
}

// HTTP handler for /data POST
esp_err_t data_handler(httpd_req_t *req) {
    char query[128];
    char id[32] = {0}, temp[32] = {0};

    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        httpd_query_key_value(query, "id", id, sizeof(id));
        httpd_query_key_value(query, "temp", temp, sizeof(temp));

        ESP_LOGI(TAG, "Received from %s: temp = %s", id, temp);

        // Append to file
        //FILE* f = fopen(STORAGE_PATH, "a");
        //if (f) {
        //    fprintf(f, "{\"id\":\"%s\",\"temp\":%s}\n", id, temp);
        //    fclose(f);
        //} else {
        //    ESP_LOGE(TAG, "Failed to open file for writing");
        //}
    }

    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

// HTTP handler for /register POST
esp_err_t register_post_handler(httpd_req_t *req) {
    char buf[200];
    int ret = httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf)));
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    char mac[18], ip[16];
    if (sscanf(buf, "{\"mac\":\"%17[^\"]\",\"ip\":\"%15[^\"]\"}", mac, ip) != 2) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Registered client: MAC=%s IP=%s", mac, ip);

    // Update or add client
    bool updated = false;
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].mac, mac) == 0) {
            strcpy(clients[i].ip, ip);
            updated = true;
            break;
        }
    }
    if (!updated && client_count < 5) {
        strcpy(clients[client_count].mac, mac);
        strcpy(clients[client_count].ip, ip);
        client_count++;
    }

    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_handle_t start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t write_pc_db_server_uri = {
            .uri = "/write_pc_db_server",
            .method = HTTP_POST,
            .handler = write_pc_db_server_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &write_pc_db_server_uri);
        
        httpd_uri_t get_pc_uri = {
            .uri = "/get_pc_ip",
            .method = HTTP_GET,
            .handler = get_pc_ip_handler
        };
        httpd_register_uri_handler(server, &get_pc_uri);

        httpd_uri_t register_uri = {
            .uri = "/register",
            .method = HTTP_POST,
            .handler = register_post_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &register_uri);
        httpd_uri_t data_uri = {
            .uri = "/data",
            .method = HTTP_POST,
            .handler = register_post_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &data_uri);
    }
    return server;
}

// ---------------------- SPIFFS Init ------------------------
void init_spiffs() {
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
    ESP_LOGI(TAG, "SPIFFS mounted.");
}

// ---------------------- WiFi AP Init ------------------------
void wifi_init_ap() {
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .ssid_len = strlen(WIFI_SSID),
            .max_connection = 10,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .ssid_hidden = 1,
            .channel = 1,
        },
    };
    if (strlen(WIFI_PASS) == 0) wifi_config.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "AP started: %s | IP: 192.168.4.1", WIFI_SSID);
}

// ---------------------- Time API Call ------------------------
esp_err_t call_time_api_and_respond(const uint8_t *mac_addr) {
    if (!main_ip) {
        ESP_LOGE(TAG, "PC IP not set. Cannot make API call.");
        return ESP_FAIL;
    }

    char url[128];
    snprintf(url, sizeof(url), "https://%s:5000/get_time", main_ip);  // Adjust path/port as needed

    // print the URL for debugging
    ESP_LOGI(TAG, "Calling time API: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 3000,
        .auth_type = HTTP_AUTH_TYPE_NONE,
        .transport_type = HTTP_TRANSPORT_OVER_TCP,
        .cert_pem = NULL
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        if (status == 200) {
            char buffer[256];
            int len = esp_http_client_read_response(client, buffer, sizeof(buffer) - 1);
            buffer[len] = '\0';

            ESP_LOGI(TAG, "API response: %s", buffer);

            cJSON *json = cJSON_Parse(buffer);
            if (!json) {
                ESP_LOGE(TAG, "Failed to parse JSON");
                esp_http_client_cleanup(client);
                return ESP_FAIL;
            }

            TimeInformation info = {
                .hour = cJSON_GetObjectItem(json, "hour")->valueint,
                .minute = cJSON_GetObjectItem(json, "minute")->valueint,
                .second = cJSON_GetObjectItem(json, "second")->valueint,
            };
            strncpy(info.date, cJSON_GetObjectItem(json, "date")->valuestring, sizeof(info.date));
            strncpy(info.day_name, cJSON_GetObjectItem(json, "day_name")->valuestring, sizeof(info.day_name));
            cJSON_Delete(json);

            // Send back via ESP-NOW
            esp_err_t result = esp_now_send(mac_addr, (uint8_t*)&info, sizeof(info));
            if (result == ESP_OK) {
                ESP_LOGI(TAG, "Time info sent via ESP-NOW");
            } else {
                ESP_LOGE(TAG, "Failed to send ESP-NOW response: %s", esp_err_to_name(result));
            }

        } else {
            ESP_LOGE(TAG, "HTTP error status: %d", status);
        }
    } else {
        ESP_LOGE(TAG, "HTTP GET failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}


// ---------------------- ESP-NOW Time Request Callback ------------------------
void on_espnow_recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
    if (!recv_info) return;

    if (data_len == sizeof(TimeRequest)) {
        TimeRequest request;
        memcpy(&request, data, sizeof(TimeRequest));

        ESP_LOGI(TAG, "Received ESP-NOW request: %s", request.requestType);

        if (strcmp(request.requestType, "Time") == 0) {
            ESP_LOGI(TAG, "TIME REQUEST RECEIVED");
            ESP_LOGI(TAG, "Waiting for time info over serial (JSON)...");

            char input_buffer[256];
            memset(input_buffer, 0, sizeof(input_buffer));

            // Wait for serial input (blocking)
            fgets(input_buffer, sizeof(input_buffer), stdin);  // works with UART stdin

            ESP_LOGI(TAG, "Received serial input: %s", input_buffer);

            // Parse JSON
            cJSON *json = cJSON_Parse(input_buffer);
            if (!json) {
                ESP_LOGE(TAG, "Failed to parse JSON from serial");
                return;
            }

            TimeInformation info = {
                .hour = cJSON_GetObjectItem(json, "hour")->valueint,
                .minute = cJSON_GetObjectItem(json, "minute")->valueint,
                .second = cJSON_GetObjectItem(json, "second")->valueint,
            };

            strncpy(info.date, cJSON_GetObjectItem(json, "date")->valuestring, sizeof(info.date));
            strncpy(info.day_name, cJSON_GetObjectItem(json, "day_name")->valuestring, sizeof(info.day_name));
            cJSON_Delete(json);

            // Send response back via ESP-NOW
            esp_err_t result = esp_now_send(recv_info->src_addr, (uint8_t*)&info, sizeof(info));
            if (result == ESP_OK) {
                ESP_LOGI(TAG, "Time info sent via ESP-NOW");
            } else {
                ESP_LOGE(TAG, "Failed to send ESP-NOW response: %s", esp_err_to_name(result));
            }

        } else {
            ESP_LOGW(TAG, "Unknown request type: %s", request.requestType);
        }
    } else if (data_len == sizeof(HRInformation)) {
        HRInformation hr_info;
        memcpy(&hr_info, data, sizeof(HRInformation));

        ESP_LOGI(TAG, "Received Heart Rate Information: HR=%d, Temp=%.2f, SpO2=%d",
                 hr_info.heart_rate, hr_info.temperature, hr_info.spo2);

        // send one by one through serial
        ESP_LOGI(SENSOR_TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X",
             recv_info->src_addr[0], recv_info->src_addr[1],
             recv_info->src_addr[2], recv_info->src_addr[3],
             recv_info->src_addr[4], recv_info->src_addr[5]);
        ESP_LOGI(SENSOR_TAG, "1: %d", hr_info.heart_rate);
        ESP_LOGI(SENSOR_TAG, "2: %.2f", hr_info.temperature);
        ESP_LOGI(SENSOR_TAG, "3: %d", hr_info.spo2);

    } else {
        ESP_LOGW(TAG, "Unknown data format or size: %d bytes", data_len);
    }
}

//
// ---------------------- Main Entry ------------------------
//
void app_main() {
    // Print MAC address
    // Get MAC address of ESP32
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_AP, mac);
    ESP_LOGI(TAG, "ESP32 MAC Address (AP): %02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // Get IP address of AP interface
    esp_netif_ip_info_t ip_info;
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
        ESP_LOGI(TAG, "ESP32 AP IP Address: " IPSTR, IP2STR(&ip_info.ip));
    }

    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

        // 1. Install UART driver
    uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

    // 2. Configure UART0 (USB)
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 3. Hook UART0 to VFS for stdin/stdout
    esp_vfs_dev_uart_use_driver(UART_NUM_0);

    //init_spiffs();
    wifi_init_ap();
    start_webserver();

    // Peer ESP-NOW configuration
    esp_now_peer_info_t peer_config = {
        .peer_addr = {0xA0, 0xDD, 0x6C, 0x72, 0x51, 0x40}, // Smartwatch MAC address
        .channel = 1,
        .encrypt = false,
    };
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_add_peer(&peer_config));
    esp_now_register_recv_cb(on_espnow_recv);

    ESP_LOGI(TAG, "ESP-NOW peer configured: %02X:%02X:%02X:%02X:%02X:%02X",
             peer_config.peer_addr[0], peer_config.peer_addr[1],
             peer_config.peer_addr[2], peer_config.peer_addr[3],
             peer_config.peer_addr[4], peer_config.peer_addr[5]);
}
