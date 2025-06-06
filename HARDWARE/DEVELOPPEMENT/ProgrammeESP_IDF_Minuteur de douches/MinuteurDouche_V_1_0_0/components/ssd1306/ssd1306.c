
#include <string.h>
#include <stdlib.h>
#include "driver/i2c.h"
#include "ssd1306.h"
#include "font6x8.h"


#define OLED_ADDR 0x3C
#define I2C_PORT I2C_NUM_0
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void ssd1306_setup_i2c(gpio_num_t sda, gpio_num_t scl)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

static void send_command(uint8_t cmd)
{
    uint8_t buffer[2] = {0x00, cmd};
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, buffer, 2, 1000 / portTICK_PERIOD_MS);
}



static void send_data(uint8_t* data, size_t len)
{
    uint8_t* buffer = malloc(len + 1);
    buffer[0] = 0x40;
    memcpy(buffer + 1, data, len);
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, buffer, len + 1, 1000 / portTICK_PERIOD_MS);
    free(buffer);
}

void ssd1306_init(void)
{
    send_command(0xAE);
    send_command(0x20);
    send_command(0x10);
    send_command(0xB0);
    send_command(0xC8);
    send_command(0x00);
    send_command(0x10);
    send_command(0x40);
    send_command(0x81);
    send_command(0x7F);
    send_command(0xA1);
    send_command(0xA6);
    send_command(0xA8);
    send_command(0x3F);
    send_command(0xA4);
    send_command(0xD3);
    send_command(0x00);
    send_command(0xD5);
    send_command(0xF0);
    send_command(0xD9);
    send_command(0x22);
    send_command(0xDA);
    send_command(0x12);
    send_command(0xDB);
    send_command(0x20);
    send_command(0x8D);
    send_command(0x14);
    send_command(0xAF);
}

void ssd1306_clear(void)
{
    uint8_t blank[OLED_WIDTH] = {0};
    for (uint8_t page = 0; page < 8; page++) {
        send_command(0xB0 + page);
        send_command(0x00);
        send_command(0x10);
        send_data(blank, OLED_WIDTH);
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t page, const char* str, uint8_t scale, bool color)
{
    while (*str) {
        if (*str < 32 || *str > 126) {
            str++;
            continue;
        }

        const uint8_t* chr = font6x8[*str - 32];

        send_command(0xB0 + page);                        // ligne (8 px)
        send_command(0x00 + (x & 0x0F));                  // colonne basse
        send_command(0x10 + ((x >> 4) & 0x0F));           // colonne haute

        send_data((uint8_t*)chr, 6);

        x += 6;
        str++;
    }
}
void ssd1306_refresh(void)
{
    // nothing
}

void ssd1306_128x64_i2c_init(void) {
    ssd1306_init();
}

void ssd1306_clear_screen(void) {
    ssd1306_clear();
}

void ssd1306_clear_line(uint8_t page) {
    uint8_t blank[128] = {0};
    send_command(0xB0 + page);
    send_command(0x00);
    send_command(0x10);
    send_data(blank, 128);
}

void ssd1306_contrast(uint8_t contrast) {
    send_command(0x81);
    send_command(contrast);
}

void ssd1306_display_text(uint8_t page, const char *text, uint8_t text_len, bool invert) {
    // Ignore 'invert' pour l’instant
    ssd1306_draw_string(0, page, text, 1, false);
}

