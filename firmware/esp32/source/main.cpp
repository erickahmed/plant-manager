#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_task_wdt.h"
#include "wifi.hpp"

volatile bool criticalErrorFlag = false;

static void watchdogTask(void *pvParameters) {
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 12000,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    ESP_ERROR_CHECK(esp_task_wdt_init(&config));
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    const TickType_t keepAlivePeriod = pdMS_TO_TICKS(8000);

    for(;;) {
        if (critical_error_flag) {
            ESP_LOGE("Critical error flag raised. Rebooting.");
            esp_restart();
        }

        esp_task_wdt_reset();

        vTaskDelay(keepAlivePeriod);
    }
}

extern "C" void app_main(void)
{
    ESP_LOGI("BOOT", "Hello from user land – bootloader worked yay!");

    static const esp_pm_configure_t pm_cfg = {
        .max_freq_mhz       = 80,
        .min_freq_mhz       = 10,
        .light_sleep_enable = true,
        .deep_sleep_enable  = true
    };
    esp_pm_configure(&pm_cfg);

    heap_caps_init();

    xTaskCreate(watchdogTask, "watchdogs", 2048, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(wifiTask, "wifi", 4096, NULL, configMAX_PRIORITIES-4, NULL);
}
