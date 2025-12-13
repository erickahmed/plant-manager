idf_component_register(
    SRCS "./main.cpp"
         "./wifi.cpp"
         "./i2c.cpp"
         "./mqtt.cpp"
    INCLUDE_DIRS "../include"
    REQUIRES esp_wifi
             nvs_flash
             esp_pm
             esp_hw_support
             esp_system
             freertos
             log
             mqtt
             esp_driver_i2c
)
