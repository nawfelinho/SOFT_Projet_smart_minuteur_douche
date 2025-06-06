#include "oled_display.h"
#include "ssd1306.h"
#include <string.h>  // pour strlen

void oled_init(void) {
    ssd1306_setup_i2c(GPIO_NUM_21, GPIO_NUM_22);  
    ssd1306_128x64_i2c_init();
    ssd1306_clear_screen();
    ssd1306_contrast(0xFF);
    ssd1306_display_text(0, "OLED OK", 8, false);
}

void oled_display_message(const char *message) {
    ssd1306_clear_line(1);
    ssd1306_display_text(1, message, strlen(message), false);
}
