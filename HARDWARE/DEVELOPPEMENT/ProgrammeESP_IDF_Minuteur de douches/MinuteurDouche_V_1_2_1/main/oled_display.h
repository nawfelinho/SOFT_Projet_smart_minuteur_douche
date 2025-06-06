/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: oled_display.h

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Interface pour la gestion de l'écran OLED via le contrôleur SSD1306 :
   - Initialisation et effacement de l'écran
   - Affichage de messages simples ou formatés
   - Dessins personnalisés (goutte, explosion)
   - Interaction utilisateur (écran d'accueil, affichage nom)

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Création initiale du header OLED pour affichage I2C
-- ========================================================================== */

#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

/**-------------------------------------------------------------------------- --
   Fonctions de base
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_init
   Initialise l’écran OLED (I2C, contraste, écran d’accueil)
-- -------------------------------------------------------------------------- */
void oled_init(void);

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_clear
   Efface entièrement l’affichage de l’écran
-- -------------------------------------------------------------------------- */
void oled_clear(void);

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_display_message
   Affiche un message simple sur une ligne centrale
   Paramètre :
     message : chaîne de caractères à afficher
-- -------------------------------------------------------------------------- */
void oled_display_message(const char *message);


/**-------------------------------------------------------------------------- --
   Fonctions d'affichage avancé
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_display_centered
   Affiche un message centré horizontalement sur une ligne donnée (1 à 8)
-- -------------------------------------------------------------------------- */
void oled_display_centered(const char *msg, uint8_t line);

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_draw_goutte
   Dessine une goutte remplie selon le pourcentage (0 à 100)
-- -------------------------------------------------------------------------- */
void oled_draw_goutte(uint8_t fill_percent);

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_draw_explosion
   Affiche une explosion visuelle (clignotante ou non selon paramètre)
-- -------------------------------------------------------------------------- */
void oled_draw_explosion(bool blink);


/**-------------------------------------------------------------------------- --
   Fonctions liées à l’utilisateur
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: show_boot_screen
   Affiche un message de bienvenue générique au démarrage
-- -------------------------------------------------------------------------- */
void show_boot_screen(void);

/* -------------------------------------------------------------------------- --
   FUNCTION: show_bienvenue_user
   Affiche un message personnalisé à l’utilisateur (nom transmis)
-- -------------------------------------------------------------------------- */
void show_bienvenue_user(const char* username);

/* -------------------------------------------------------------------------- --
   FUNCTION: on_username_received
   Callback appelé lors de la réception d’un nom utilisateur via BLE
-- -------------------------------------------------------------------------- */
void on_username_received(const char* username);


/**-------------------------------------------------------------------------- --
   Variable globale
-- -------------------------------------------------------------------------- */

/* Nom de l’utilisateur connecté, mis à jour via BLE */
extern char user_name[32];

#endif // OLED_DISPLAY_H
