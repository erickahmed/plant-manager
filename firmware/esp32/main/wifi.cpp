#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "main.hpp"
#include "wifi-credentials.h"

#define WIFI_TIMEOUT_TICKS pdMS_TO_TICKS(8000)

static const char* TAG = "WIFI";
const int WIFI_CONNECTED_BIT = BIT0;

EventGroupHandle_t wifi_event_group;

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "Wi-Fi process started, connecting...");

                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                wifi_event_sta_disconnected_t* disconn = (wifi_event_sta_disconnected_t*) event_data;
                ESP_LOGW(TAG, "Disconnected, reason: %d", disconn->reason);

                xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
                esp_wifi_connect();
                break;
            }
            default:
                break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init_sta(void) {
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

    for(;;) {
        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGI(TAG, "Wi-Fi task reset");
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}
