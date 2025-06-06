#ifndef SSD1306_H
#define SSD1306_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/i2c.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

void ssd1306_setup_i2c(gpio_num_t sda, gpio_num_t scl);
void ssd1306_128x64_i2c_init(void);
void ssd1306_clear_screen(void);
void ssd1306_clear_line(uint8_t page);
void ssd1306_contrast(uint8_t contrast);
void ssd1306_display_text(uint8_t page, const char *text, uint8_t text_len, bool invert);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_H
