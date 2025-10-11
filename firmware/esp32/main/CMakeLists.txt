idf_component_register(SRCS "../source/main.cpp"
                            "../source/wifi.cpp"
                       INCLUDE_DIRS "../include"
                       REQUIRES esp_wifi
                                nvs_flash
                                esp_pm
                                esp_hw_support
                                esp_system
                                freertos
                                log)
