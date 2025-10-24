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

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch(event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                esp_wifi_connect();
                xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
                break;
        }
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
