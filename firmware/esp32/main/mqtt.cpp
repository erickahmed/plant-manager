#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "main.hpp"
//#include "i2c.hpp"

#define MQTT_TASK_TIMEOUT_MS 3000

static const char* TAG = "MQTT";

static TaskHandle_t i2c_task_handle = NULL;
static esp_mqtt_client_handle_t mqtt_client = NULL;

static char global_rx_buffer[50];

static inline void mqtt_subscribe(void) {
    esp_mqtt_client_subscribe(mqtt_client, "/topic/qos0", 0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED: {
            xEventGroupSetBits(connectivity_event_group, MQTT_CONNECTED_BIT);
            mqtt_subscribe();
            ESP_LOGV(TAG, "Connected to broker");
            break;
        }
        case MQTT_EVENT_DISCONNECTED: {
            xEventGroupClearBits(connectivity_event_group, MQTT_CONNECTED_BIT);
            ESP_LOGW(TAG, "Disconnected from broker");
            break;
        }
        case MQTT_EVENT_DATA: {
            ESP_LOGD(TAG, "Listening to broker");
            ESP_LOGD(TAG, "Topic: %.*s\n", event->topic_len, event->topic);
            ESP_LOGD(TAG, "Data:  %.*s\n", event->data_len, event->data);

            int len = event->data_len;
            if (len > sizeof(global_rx_buffer) - 1) len = sizeof(global_rx_buffer) - 1;
            memcpy(global_rx_buffer, event->data, len);
            global_rx_buffer[len] = '\0';

            if (i2c_task_handle != NULL) xTaskNotifyGive(i2c_task_handle);
            break;
        }
        default: {
            break;
        }
    }
}

static void mqtt_init(void) {
    // TODO; implement Trust only or Mutual TLS in the future (and secure boot + flash encryption)
    // TODO: add to config.h
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = "mqtt://yourmqttserver";
    mqtt_cfg.broker.address.port = 1883;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqttTask(void *pvParameters) {
    ESP_LOGI(TAG, "Task started");

    i2c_task_handle = xTaskGetCurrentTaskHandle();
    mqtt_init();

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for(;;) {
        ESP_LOGV(TAG, "Checking broker connection...");

        xEventGroupWaitBits(connectivity_event_group, WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

        ESP_LOGV(TAG, "Connection established");

        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(MQTT_TASK_TIMEOUT_MS))) {
            //i2c_do(global_rx_buffer); PLACEHOLDER FUNC!
        }

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGV(TAG, "Task reset");
     }
    ESP_LOGI(TAG, "Task stopped");
}
