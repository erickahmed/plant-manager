#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "main.hpp"
#include "wifi-credentials.h"

static const char* TAG = "WIFI";

EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static void wifiInit() {
    ESP_LOGI(TAG, "Initializing WiFi...");

    memset(&wifi_cfg, 0, sizeof(wifi_cfg));
    strcpy((char*)wifi_cfg.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_cfg.sta.password, WIFI_PASSWORD);
    wifi_cfg.sta.threshold.authmode = WIFI_AUTH;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

    ESP_ERROR_CHECK(esp_wifi_start());
}

static inline void wifiTryConnect() {
    wifiInit();

    wifi_mode_t mode;

    if(esp_wifi_get_mode(&mode) == ESP_OK && mode == WIFI_MODE_STA) {
        ESP_LOGI(TAG, "Attempting to connect to WiFi...");
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else {
        ESP_LOGE(TAG, "WiFi mode check failed, setting critical error flag");
        criticalErrorFlag = true;
    }
}

static void wifiManager() {
    wifi_mode_t mode;
    if(esp_wifi_get_mode(&mode) != ESP_OK || mode != WIFI_MODE_STA) {
        wifiTryConnect();
    }
}

void wifiTask(void *pvParameters) {
    ESP_LOGI(TAG, "WiFi task started");
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    wifiManager();

    for(;;) {
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}
