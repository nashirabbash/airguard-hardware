idf_component_register(
    SRCS "main.cpp" "log.cpp" "../drivers/dht22.cpp" "../drivers/mq135.cpp" "../drivers/mod.cpp"
    INCLUDE_DIRS "." "../drivers"
    REQUIRES esp_event esp_netif esp_system freertos lwip dht
)
