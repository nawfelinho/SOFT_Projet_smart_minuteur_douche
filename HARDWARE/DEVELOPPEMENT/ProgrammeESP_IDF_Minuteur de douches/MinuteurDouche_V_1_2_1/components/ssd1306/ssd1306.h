/* ========================================================================== --
                  Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: ssd1306.h

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Fichier d'en-tête pour l'afficheur OLED SSD1306 en I2C (128x64).
   Déclare les fonctions de configuration, d’affichage de texte,
   de dessin pixel à pixel, et de gestion d’un framebuffer.

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 2025-06-06 - Authors: Damien LORIGEON / Projet Minuteur ESP32
   + Définition des prototypes du pilote OLED
-- ========================================================================== */

#ifndef SSD1306_H
#define SSD1306_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/i2c.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/*                      Fonctions de configuration / init                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialise le port I2C avec les broches spécifiées
 * @param sda Broche SDA
 * @param scl Broche SCL
 */
void ssd1306_setup_i2c(gpio_num_t sda, gpio_num_t scl);

/**
 * @brief Initialise l’écran OLED SSD1306 (128x64) via I2C
 */
void ssd1306_128x64_i2c_init(void);

/* -------------------------------------------------------------------------- */
/*                           Fonctions de nettoyage                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Efface complètement l’écran
 */
void ssd1306_clear_screen(void);

/**
 * @brief Efface une ligne (page) de l’écran (8 pixels de haut)
 * @param page Numéro de la ligne (0 à 7)
 */
void ssd1306_clear_line(uint8_t page);

/**
 * @brief Régle la luminosité de l’écran (contraste)
 * @param contrast Valeur entre 0x00 (noir) et 0xFF (max)
 */
void ssd1306_contrast(uint8_t contrast);

/* -------------------------------------------------------------------------- */
/*                           Fonctions d'affichage                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Affiche une chaîne de caractères ASCII sur une ligne donnée
 * @param page Ligne (0 à 7)
 * @param text Pointeur vers la chaîne à afficher
 * @param text_len Longueur maximale (non utilisée ici)
 * @param invert Inverser les pixels (non supporté pour le moment)
 */
void ssd1306_display_text(uint8_t page, const char *text, uint8_t text_len, bool invert);

/* -------------------------------------------------------------------------- */
/*                           Fonctions graphiques                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Dessine un pixel dans le framebuffer
 * @param x Coordonnée horizontale (0 à 127)
 * @param y Coordonnée verticale (0 à 63)
 * @param color true = pixel blanc / false = noir
 */
void ssd1306_draw_pixel(int x, int y, bool color);

/**
 * @brief Dessine un rectangle plein à l’écran
 * @param x Coin supérieur gauche X
 * @param y Coin supérieur gauche Y
 * @param w Largeur du rectangle
 * @param h Hauteur du rectangle
 * @param color Couleur (true = blanc)
 */
void ssd1306_draw_rect(int x, int y, int w, int h, bool color);

/**
 * @brief Dessine une image bitmap 1bpp dans le framebuffer
 * @param x Position X
 * @param y Position Y
 * @param w Largeur (en pixels)
 * @param h Hauteur (en pixels)
 * @param data Tableau contenant l’image (1 bit/pixel)
 */
void ssd1306_draw_bitmap(int x, int y, int w, int h, const uint8_t* data);

/* -------------------------------------------------------------------------- */
/*                         Fonctions sur le framebuffer                       */
/* -------------------------------------------------------------------------- */

/**
 * @brief Efface complètement le framebuffer local
 */
void ssd1306_fb_clear(void);

/**
 * @brief Transmet le contenu du framebuffer à l’écran OLED
 */
void ssd1306_fb_flush(void);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_H
