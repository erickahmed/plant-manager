#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_pm.h"

volatile bool critical_error_flag = false;

static void watchdogTask(void pvParameters*) {
    const TickType_t keepAlivePeriod = pdMS_TO_TICKS(30000);

    for(;;) {
        if (critical_error_flag) {
            ESPLOGE("Critical error flag raised. Rebooting.");
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

    xTaskCreate(watchdogTask, "wdt", 256, NULL, configMAX_PRIORITIES-1, NULL);
}
