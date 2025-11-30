#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "main.hpp"

static const char* TAG = "MQTT";


static void mqtt_init(void) {
    // TODO; implement Trust only or Mutual TLS in the future (and secure boot + flash encryption)
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            //TODO: make this a definition in config.hpp
            .address.uri = "mqtt://yourmqttserver",
            .address.port = 1883
        },

        esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
        mqttClient = client;
        /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
        esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
        esp_mqtt_client_start(client);
}

static void mqtt_subscribe(void) {
    msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
}

static void mqtt_publish(void) {}

static void mqtt_fetch(void) {}

void mqttTask(void *pvParameters) {
    ESP_LOGI(TAG, "Task started");

    mqtt_init();
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for(;;) {
        ESP_LOGI(TAG, "Checking Wi-Fi connection...");
        xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT) {
            mqtt_publish();
            mqtt_fetch();
        }

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGI(TAG, "Task reset");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
