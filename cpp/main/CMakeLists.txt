idf_component_register(
    SRCS "main.cpp" "log.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_event esp_netif esp_system freertos lwip
)
