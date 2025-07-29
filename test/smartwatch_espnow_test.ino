
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

#define DEBUG

#ifdef DEBUG
  #define debugPrint(x) Serial.print(x)
  #define debugPrintln(x) Serial.println(x)
  #define debugPrintfHex(x) Serial.printf("%02X", x)
#else
  #define debugPrint(x)
  #define debugPrintln(x)
  #define debugPrintfHex(x)
#endif


struct SensorData {
  char sensorType[36];
  float value;
};

struct TimeInformation {
  char day_name[32];
  char date[32];
  int hour;
  int minute;
  int second;
};

struct TimeRequest {
  char requestType[5];  // Example: "Time"
};

struct WeatherData {
  char city[32];
  char weather_description[32];
  float temperature;
  int humidity;
  float wind_speed;
};

struct WeatherRequest {
  char requestType[16];  // Example: "Current", "Weekly"
};

struct PatientData {
  int beat_avg;
  float temperature;
  int32_t spo2;
};

struct PatientIRDataParted {
  uint32_t sample_hr_plot[50];
  int sample_hr_plot_spot;
};

static const char* PMK_KEY_STR = "U@Kw%nV&PzbRK2WM";
static const char* LMK_KEY_STR = "Ruth:WqH9@j%h5qg";

uint8_t peer_mac_addresses[3][6] = {
  {0xA0, 0xB7, 0x65, 0x28, 0xCE, 0xA0}, // Device 1 MAC
  {0xA0, 0xDD, 0x6C, 0x72, 0x51, 0x40}, // Watch MAC A0:DD:6C:72:51:40
  {0xE0, 0xE2, 0xE6, 0x63, 0x0F, 0x50}
  /////////// ...
};

void onDataRecv(const esp_now_recv_info_t* messageInfo, const uint8_t* incomingData, int len) {
  // Debug Print: MAC Address of Sender
  debugPrint("Received packet from MAC: ");
  for (int i = 0; i < 6; i++) {
    debugPrintfHex(messageInfo->src_addr[i]);
    if (i < 5) debugPrint(":");
  }
  debugPrintln();

  debugPrint("Received packet from MAC: ");
  for (int i = 0; i < 6; i++) {
    debugPrintfHex(messageInfo->des_addr[i]);
    if (i < 5) debugPrint(":");
  }
  debugPrintln();

  debugPrint("Raw Data Length: ");
  debugPrintln(len);

  // Check if the received data is a WeatherRequest
  if (len == sizeof(WeatherRequest)) {
    WeatherRequest receivedRequest;
    memcpy(&receivedRequest, incomingData, sizeof(receivedRequest));

    debugPrint("Received Request Type: ");
    debugPrintln(receivedRequest.requestType);

    // Handle the WeatherRequest
    if (strcmp(receivedRequest.requestType, "Current") == 0) {
      debugPrintln("Handling Current Weather Request");

      // Create a dummy WeatherData response
      WeatherData dummyWeather;
      strncpy(dummyWeather.city, "Ankara", sizeof(dummyWeather.city));
      strncpy(dummyWeather.weather_description, "clear sky", sizeof(dummyWeather.weather_description));
      dummyWeather.temperature = 22.5;
      dummyWeather.humidity = 60;
      dummyWeather.wind_speed = 5.3;

      uint8_t watch_mac[] = {0xA0, 0xDD, 0x6C, 0x72, 0x51, 0x40};
      // Send the dummy WeatherData back to the sender
      esp_err_t result = esp_now_send(messageInfo->src_addr, (uint8_t*)&dummyWeather, sizeof(dummyWeather));

      if (result == ESP_OK) {
        debugPrintln("Dummy WeatherData sent successfully.");
      } else {
        debugPrintln("Failed to send Dummy WeatherData.");
      }

    } else if (strcmp(receivedRequest.requestType, "Weekly") == 0) {
      debugPrintln("Handling Weekly Weather Request");
      // You can add logic here to handle weekly requests if needed
    } else {
      debugPrintln("Unknown Request Type");
    }
  } else if (len == sizeof(TimeRequest)) {
    TimeRequest timeRequest;
    memcpy(&timeRequest, incomingData, sizeof(timeRequest));

    debugPrint("Received Request Type: ");
    debugPrintln(timeRequest.requestType);

    // Handle the WeatherRequest
    if (strcmp(timeRequest.requestType, "Time") == 0) {
      debugPrintln("Handling Time Request");

      // Create a dummy WeatherData response
      TimeInformation dummyTime;
      strncpy(dummyTime.date, "13.02.2025", sizeof(dummyTime.date));
      strncpy(dummyTime.day_name, "Sali", sizeof(dummyTime.day_name));
      dummyTime.hour = 13;
      dummyTime.minute = 47;
      dummyTime.second = 11;

      esp_err_t result = esp_now_send(messageInfo->src_addr, (uint8_t*)&dummyTime, sizeof(dummyTime));

      if (result == ESP_OK) {
        debugPrintln("Dummy TimeInformation sent successfully.");
      } else {
        debugPrintln("Failed to send Dummy TimeInformation.");
      }
    }

  } else if (len == sizeof(SensorData)) {
    // Check if the received data is SensorData
    SensorData receivedData;
    memcpy(&receivedData, incomingData, sizeof(receivedData));

    debugPrint("Received Sensor Type: ");
    debugPrintln(receivedData.sensorType);
    debugPrint("Received Value: ");
    debugPrintln(receivedData.value);

    // You can add further logic here to handle the SensorData
  } else if (len == sizeof(PatientData)) {
    PatientData receivedPatientData;
    memcpy(&receivedPatientData, incomingData, sizeof(receivedPatientData));

    debugPrint("Received Patient Data beat_avg: ");
    debugPrintln(receivedPatientData.beat_avg);
    debugPrint("Received Patient Data temperature: ");
    debugPrintln(receivedPatientData.temperature);
    debugPrint("Received Patient Data spo2: ");
    debugPrintln(receivedPatientData.spo2);
    debugPrint("Received Patient Data ir_plot: ");
    debugPrintln("received");
  } else if (len == sizeof(PatientIRDataParted)) {
    PatientIRDataParted receivedPatientIRDataParted;
    memcpy(&receivedPatientIRDataParted, incomingData, sizeof(receivedPatientIRDataParted));

    debugPrint("Received Patient Data Part: ");
    debugPrintln(receivedPatientIRDataParted.sample_hr_plot_spot);
  } else {
    debugPrintln("Received data does not match any known structure.");
  }
}

uint8_t senderMAC[] = {0xA0, 0xDD, 0x6C, 0x72, 0x51, 0x40}; // Replace with sender's MAC address
void addSenderAsPeer() {
  for (int i = 0; i < 2; i++) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peer_mac_addresses[i], 6);
    peerInfo.encrypt = true;

    for (uint8_t i = 0; i < 16; i++) {
      peerInfo.lmk[i] = LMK_KEY_STR[i];
    }

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      debugPrintln("Failed to add sender as a peer.");
    } else {
      debugPrintln("Sender added as a peer.");
    }
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
      baseMac[0], baseMac[1], baseMac[2],
      baseMac[3], baseMac[4], baseMac[5]);
  } else {
    debugPrintln("Failed to read MAC address");
  }

  if (esp_now_init() != ESP_OK) {
    debugPrintln("ESP-NOW Initialization Failed.");
    return;
  } else {
    debugPrintln("ESP-NOW Initialized Successfully.");
  }

  esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
  addSenderAsPeer();
  
  esp_now_register_recv_cb(onDataRecv);
}
 
void loop() {

}