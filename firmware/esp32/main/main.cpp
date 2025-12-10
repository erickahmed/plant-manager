#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_task_wdt.h"
#include "wifi.hpp"
#include "i2c.cpp"
#include "mqtt.hpp"

#define WATCHDOG_KEEPALIVE_MS 8000

EventGroupHandle_t connectivity_event_group;

static const char* TAG = "MAIN";

volatile bool criticalErrorFlag = false;

static void watchdogTask(void *pvParameters) {
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 12000,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    ESP_ERROR_CHECK(esp_task_wdt_deinit());
    ESP_ERROR_CHECK(esp_task_wdt_init(&twdt_config));

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for(;;) {
        if(criticalErrorFlag) {
            ESP_LOGE(TAG, "Critical error flag raised. Rebooting.");
            esp_restart();
        }

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGV(TAG, "WDT reset");

        vTaskDelay(pdMS_TO_TICKS(WATCHDOG_KEEPALIVE_MS));
    }
}

extern "C" void app_main(void)
{
    static const esp_pm_config_t pm_cfg = {
        .max_freq_mhz       = 160,
        .min_freq_mhz       = 10,
        .light_sleep_enable = true
    };
    esp_pm_configure(&pm_cfg);

    connectivity_event_group = xEventGroupCreate();

    xTaskCreate(watchdogTask, "watchdogs", 2048, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(wifiTask, "wifi", 4096, NULL, configMAX_PRIORITIES-4, NULL);
    xTaskCreate(i2cTask, "i2c", 1024, NULL, configMAX_PRIORITIES-6, &i2c_task_handle);
    xTaskCreate(mqttTask, "mqtt", 2048, NULL, configMAX_PRIORITIES-8, NULL);
}
