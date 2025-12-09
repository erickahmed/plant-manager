#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_log.h"

// ESP32-C3 Super Mini, change this ports for other versions
#define I2C_MASTER_SCL_IO 9
#define I2C_MASTER_SDA_IO 8

// TODO: put this in a config.hpp
// Change based on how many vases max to manage [1 to ~100]
// larger number will require larger array
// see limitation on AVR addresses for TWI
#define SLAVES_NUMBER 32

static const char* TAG = "I2C";

TaskHandle_t i2c_task_handle = NULL;

i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t slave_handles[SLAVES_NUMBER];
uint8_t slave_count = 0;

static void register_slave(int addr) {
    i2c_device_config_t slave_dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000, // 100kHz or 400kHz
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &slave_dev_cfg, &slave_handles[slave_count]));
    slave_count++;
}

static void i2c_init(void) {
    i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = -1,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    // Add all vases adresses here:
    // TODO: move this to a config.cpp
    register_slave(0x30);
}

static void i2c_read() {} //should read from i2c and if necessary call i2c_write
static void i2c_write() {}

void i2cTask(void *pvParameters) {
    ESP_LOGI(TAG, "Task started");
    i2c_init();

    for(;;) {
        // TODO: this should check for MQTT connection (which in turn checks for WI-FI)
        ESP_LOGI(TAG, "Checking Wi-Fi connection...");
        //FIXME: change wifi_event_group to connectivity_event_group after merge with dev-esp32-mqtt branch
        xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

        if ((bits & (WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT)) == (WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT)) i2c_read();

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGI(TAG, "Task reset");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
