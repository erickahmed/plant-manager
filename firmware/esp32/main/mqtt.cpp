#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "main.hpp"
#include "i2c.hpp"

#define MQTT_TASK_TIMEOUT_MS 3000
#define MQTT_TIMEOUT_MS MQTT_TASK_TIMEOUT_MS/2

static const char* TAG = "MQTT";

static TaskHandle_t i2c_task_handle = NULL;
static esp_mqtt_client_handle_t mqtt_client = NULL;

static void mqtt_subscribe(void) {
    msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(connectivity_event_group, MQTT_CONNECTED_BIT);
            mqtt_subscribe();
            ESP_LOGV(TAG, "Connected to broker");
            break;
        case MQTT_EVENT_DISCONNECTED:
            xEventGroupClearBits(connectivity_event_group, MQTT_CONNECTED_BIT);
            ESP_LOGW(TAG, "Disconnected from broker");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGD(TAG, "Listening to broker");
            ESP_LOGD(TAG, "Topic: %.*s\n", event->topic_len, event->topic);
            ESP_LOGD(TAG, "Data:  %.*s\n", event->data_len, event->data);

            char incoming_message[50]; //FIXME: check how long is the longest message on AVR!

            memcpy(incoming_message, event->data, event->data_len);
            incoming_message[event->data_len] = '\0'; //WARNING: maybe add some checks to the data_len
            xTaskNotifyGive(i2c_task_handle);
            break;
        default:
            break;
    }
}

static void mqtt_init(void) {
    // TODO; implement Trust only or Mutual TLS in the future (and secure boot + flash encryption)
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            //TODO: make this a definition in config.hpp
            .address.uri = "mqtt://yourmqttserver",
            .address.port = 1883
        },
    };

        esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
        mqttClient = client;
        // the last argument may be used to pass data to the event handler
        esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
        esp_mqtt_client_start(client);
}

void mqttTask(void *pvParameters) {
    ESP_LOGV(TAG, "Task started");

    i2c_task_handle = xTaskGetCurrentTaskHandle();

    mqtt_init();

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for(;;) {
        ESP_LOGV(TAG, "Checking broker connection...");

        EventBits_t bits = xEventGroupGetBits(connectivity_event_group);

        if ((bits & (WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT)) == (WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT)) {
            ESP_LOGV(TAG, "Connection established");

            uint32_t task_notification = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(MQTT_TASK_TIMEOUT_MS));
            if (task_notification) twi_do(global_rx_buffer); //FIXME: develop i2c libs! (this func should then call mqtt_publish() if needed)
        } else {
            ESP_LOGW(TAG, "Waiting for connection...");
            vTaskDelay(pdMS_TO_TICKS(MQTT_TIMEOUT_MS));
        }

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGV(TAG, "Task reset");
    }
}
