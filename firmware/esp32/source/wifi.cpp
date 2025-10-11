#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"

void wifiTask(void *pvParameters) {
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    // wifi here
    wifiConnection();

    esp_task_wdt_reset();
    vTaskDelay(pdMS_TO_TICKS(2500));
}
