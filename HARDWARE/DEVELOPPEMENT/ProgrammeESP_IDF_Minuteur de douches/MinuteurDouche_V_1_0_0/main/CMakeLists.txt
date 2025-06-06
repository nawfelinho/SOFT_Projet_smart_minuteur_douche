idf_component_register(
    SRCS 
        "ble_spp_server.c"
        "button_handler.c"
        "oled_display.c"
        "main.c"
    INCLUDE_DIRS 
        "."
    PRIV_REQUIRES 
        ssd1306
        nvs_flash
        driver
        bt
        
        
)
