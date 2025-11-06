#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "main.hpp"

static const char* TAG = "MQTT";

static void mqtt_init(void) {}

static void mqtt_publish(void) {}

void mqttTask(void *pvParameters) {
    ESP_LOGI(TAG, "Task started");

    mqtt_init();
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));


    for(;;) {
        ESP_LOGI(TAG, "Checking Wi-Fi connection...");
        xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT) mqtt_publish();

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGI(TAG, "Task reset");
        vTaskDelay(pdMS_TO_TICKS(6000));
    }
}
