#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "main.hpp"
#include "wifi-credentials.h"

#define WIFI_TIMEOUT_TICKS pdMS_TO_TICKS(30000)

static const char* TAG = "WIFI";
const int WIFI_CONNECTED_BIT = BIT0;

EventGroupHandle_t wifi_event_group;

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
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
}

void wifi_init_sta(void) {
    esp_err_t nvs_err = nvs_flash_init();

    if (nvs_err == ESP_ERR_NVS_NO_FREE_PAGES || nvs_err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config{};
    strncpy((char*)wifi_config.sta.ssid, SSID, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, PASSWORD, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = AUTH_MODE;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifiTask(void *pvParameters) {
    ESP_LOGI(TAG, "WiFi task started");

    wifi_init_sta();
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    TickType_t startTick = xTaskGetTickCount();
    do {
        EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Connecting to Wi-Fi...");

        if (bits & WIFI_CONNECTED_BIT) break;

        ESP_LOGI(TAG, "Connection attempt was unsuccessful");
        vTaskDelay(pdMS_TO_TICKS(250));
        } while ((xTaskGetTickCount() - startTick) < WIFI_TIMEOUT_TICKS);

    if (xEventGroupGetBits(wifi_event_group) & WIFI_CONNECTED_BIT) ESP_LOGI(TAG, "Connection attempt was successful");
    else {
        ESP_LOGE(TAG, "Critical error: Wi-Fi network refused connection or not found");
        criticalErrorFlag = true;
    }

    esp_task_wdt_reset();
    ESP_LOGI(TAG, "Wi-Fi reset");
    vTaskDelay(pdMS_TO_TICKS(4000));
}
