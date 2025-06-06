/* ========================================================================== --
                  Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: ssd1306.c

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Pilote pour l’écran OLED SSD1306 via I2C (format 128x64).
   Fournit des fonctions d’initialisation, d’affichage de texte, et de
   gestion d’un framebuffer pixel à pixel.

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 2025-06-06 - Authors: Damien LORIGEON / Projet Minuteur ESP32
   + création du pilote complet avec prise en charge du texte et du framebuffer
-- ========================================================================== */

/** -------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include <string.h>
#include <stdlib.h>
#include "driver/i2c.h"
#include "ssd1306.h"
#include "font6x8.h"

/** -------------------------------------------------------------------------- --
   Macro definitions
-- -------------------------------------------------------------------------- */
#define OLED_ADDR     0x3C            /* Adresse I2C de l’OLED */
#define I2C_PORT      I2C_NUM_0       /* Port I2C utilisé */
#define OLED_WIDTH    128             /* Largeur en pixels */
#define OLED_HEIGHT   64              /* Hauteur en pixels */

/** -------------------------------------------------------------------------- --
   Initialisation du port I2C
-- -------------------------------------------------------------------------- */
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

/** -------------------------------------------------------------------------- --
   Envoi d’une commande à l’écran
-- -------------------------------------------------------------------------- */
static void send_command(uint8_t cmd)
{
    uint8_t buffer[2] = {0x00, cmd};  // 0x00 = type commande
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, buffer, 2, 1000 / portTICK_PERIOD_MS);
}

/** -------------------------------------------------------------------------- --
   Envoi de données (ex: lignes entières, caractères)
-- -------------------------------------------------------------------------- */
static void send_data(uint8_t* data, size_t len)
{
    uint8_t* buffer = malloc(len + 1);
    buffer[0] = 0x40; // 0x40 = type data
    memcpy(buffer + 1, data, len);
    i2c_master_write_to_device(I2C_PORT, OLED_ADDR, buffer, len + 1, 1000 / portTICK_PERIOD_MS);
    free(buffer);
}

/** -------------------------------------------------------------------------- --
   Initialisation complète du SSD1306
-- -------------------------------------------------------------------------- */
void ssd1306_init(void)
{
    send_command(0xAE); // Display off
    send_command(0x20); send_command(0x10); // Set Memory Addressing Mode
    send_command(0xB0); // Set page start address
    send_command(0xC8); // COM Output Scan Direction
    send_command(0x00); send_command(0x10); // Set low/high column address
    send_command(0x40); // Start line address
    send_command(0x81); send_command(0x7F); // Contrast control
    send_command(0xA1); // Segment re-map
    send_command(0xA6); // Normal display
    send_command(0xA8); send_command(0x3F); // Multiplex ratio
    send_command(0xA4); // Entire display on, resume to RAM content
    send_command(0xD3); send_command(0x00); // Display offset
    send_command(0xD5); send_command(0xF0); // Display clock divide
    send_command(0xD9); send_command(0x22); // Pre-charge period
    send_command(0xDA); send_command(0x12); // COM pins hardware config
    send_command(0xDB); send_command(0x20); // VCOMH deselect level
    send_command(0x8D); send_command(0x14); // Enable charge pump
    send_command(0xAF); // Display on
}

/** -------------------------------------------------------------------------- --
   Efface entièrement l’écran
-- -------------------------------------------------------------------------- */
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

/** -------------------------------------------------------------------------- --
   Affiche une chaîne de caractères ASCII 6x8 sur une page donnée
-- -------------------------------------------------------------------------- */
void ssd1306_draw_string(uint8_t x, uint8_t page, const char* str, uint8_t scale, bool color)
{
    while (*str) {
        if (*str < 32 || *str > 126) {
            str++;
            continue;
        }

        const uint8_t* chr = font6x8[*str - 32];

        send_command(0xB0 + page);
        send_command(0x00 + (x & 0x0F));
        send_command(0x10 + ((x >> 4) & 0x0F));

        send_data((uint8_t*)chr, 6);

        x += 6;
        str++;
    }
}

/** -------------------------------------------------------------------------- --
   Stub pour compatibilité (non utilisé ici)
-- -------------------------------------------------------------------------- */
void ssd1306_refresh(void) { }

/** -------------------------------------------------------------------------- --
   Alias pour initialiser rapidement l’écran
-- -------------------------------------------------------------------------- */
void ssd1306_128x64_i2c_init(void) {
    ssd1306_init();
}

/** -------------------------------------------------------------------------- --
   Efface tout l’écran (alias)
-- -------------------------------------------------------------------------- */
void ssd1306_clear_screen(void) {
    ssd1306_clear();
}

/** -------------------------------------------------------------------------- --
   Efface une seule ligne (page de 8 pixels)
-- -------------------------------------------------------------------------- */
void ssd1306_clear_line(uint8_t page) {
    uint8_t blank[128] = {0};
    send_command(0xB0 + page);
    send_command(0x00);
    send_command(0x10);
    send_data(blank, 128);
}

/** -------------------------------------------------------------------------- --
   Réglage de la luminosité (contraste)
-- -------------------------------------------------------------------------- */
void ssd1306_contrast(uint8_t contrast) {
    send_command(0x81);
    send_command(contrast);
}

/** -------------------------------------------------------------------------- --
   Affiche une ligne de texte (texte brut uniquement)
-- -------------------------------------------------------------------------- */
void ssd1306_display_text(uint8_t page, const char *text, uint8_t text_len, bool invert) {
    // Ignore l’inversion pour l’instant
    ssd1306_draw_string(0, page, text, 1, false);
}

/** -------------------------------------------------------------------------- --
   FRAMEBUFFER : buffer mémoire (128 x 64 / 8 = 1024 octets)
-- -------------------------------------------------------------------------- */
static uint8_t ssd1306_fb[128 * 8] = {0};

/** -------------------------------------------------------------------------- --
   Efface le framebuffer
-- -------------------------------------------------------------------------- */
void ssd1306_fb_clear(void) {
    memset(ssd1306_fb, 0, sizeof(ssd1306_fb));
}

/** -------------------------------------------------------------------------- --
   Dessine un pixel dans le framebuffer
-- -------------------------------------------------------------------------- */
void ssd1306_draw_pixel(int x, int y, bool color) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
    uint16_t byte_idx = x + (y/8)*128;
    uint8_t bit = 1 << (y % 8);
    if (color)
        ssd1306_fb[byte_idx] |= bit;
    else
        ssd1306_fb[byte_idx] &= ~bit;
}

/** -------------------------------------------------------------------------- --
   Dessine un rectangle rempli (framebuffer uniquement)
-- -------------------------------------------------------------------------- */
void ssd1306_draw_rect(int x, int y, int w, int h, bool color) {
    for (int dx = 0; dx < w; dx++)
        for (int dy = 0; dy < h; dy++)
            ssd1306_draw_pixel(x+dx, y+dy, color);
}

/** -------------------------------------------------------------------------- --
   Affiche une image bitmap monochrome 1bpp dans le framebuffer
-- -------------------------------------------------------------------------- */
void ssd1306_draw_bitmap(int x, int y, int w, int h, const uint8_t* data) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            int byte = (j * w + i) / 8;
            int bit = 7 - ((j * w + i) % 8);
            bool pixel = (data[byte] >> bit) & 1;
            ssd1306_draw_pixel(x + i, y + j, pixel);
        }
    }
}

/** -------------------------------------------------------------------------- --
   Transfert du framebuffer complet à l’écran
-- -------------------------------------------------------------------------- */
void ssd1306_fb_flush(void) {
    for (uint8_t page = 0; page < 8; page++) {
        send_command(0xB0 + page);
        send_command(0x00);
        send_command(0x10);
        send_data(&ssd1306_fb[page * 128], 128);
    }
}
