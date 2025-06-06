/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: oled_display.c

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Fonctions de gestion de l’écran OLED 128x64 via le pilote SSD1306 :
   - Initialisation de l’écran OLED via I2C (GPIO 21/22)
   - Fonctions d’affichage de texte centré, de messages, de goutte animée,
     d’écran d’explosion et d’écran de bienvenue
   - Réception du nom d’utilisateur via BLE et affichage

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Première version fonctionnelle OLED avec personnalisation BLE

-- ========================================================================== */

/**-------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include "oled_display.h"
#include "ssd1306.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**-------------------------------------------------------------------------- --
   Global variables
-- -------------------------------------------------------------------------- */
char user_name[32] = "";   // Nom d’utilisateur courant (reçu via BLE)

/**========================================================================== --
   Public functions
-- ========================================================================== */

/* -------------------------------------------------------------------------- --
   FUNCTION: oled_init

   --------------------------------------------------------------------------
   Purpose:
   Initialisation de l’écran OLED via le protocole I2C

   --------------------------------------------------------------------------
   Description:
   - Configure l’interface I2C sur GPIO21 (SDA) / GPIO22 (SCL)
   - Initialise l’écran SSD1306 en 128x64
   - Efface l’écran et applique un contraste maximal
   - Affiche un message d’état centré

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void oled_init(void) {
    ssd1306_setup_i2c(GPIO_NUM_21, GPIO_NUM_22);   // Initialisation I2C
    ssd1306_128x64_i2c_init();                     // Init du SSD1306
    ssd1306_clear_screen();                        // Écran vide
    ssd1306_contrast(0xFF);                        // Contraste fort
    oled_display_centered(" Minuteur ESP32 Connecté ", 3);  // Message par défaut
}


/* -------------------------------------------------------------------------- --
   FUNCTION: oled_clear

   --------------------------------------------------------------------------
   Purpose:
   Efface complètement l’écran OLED

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void oled_clear(void) {
    ssd1306_clear_screen();
}


/* -------------------------------------------------------------------------- --
   FUNCTION: oled_display_message

   --------------------------------------------------------------------------
   Purpose:
   Affiche un message texte simple sur l’écran OLED

   --------------------------------------------------------------------------
   Parameters:
     message : Chaîne de caractères à afficher

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void oled_display_message(const char *message) {
    oled_clear();
    ssd1306_display_text(3, message, strlen(message), false);
}


/* -------------------------------------------------------------------------- --
   FUNCTION: oled_display_centered

   --------------------------------------------------------------------------
   Purpose:
   Afficher un message centré horizontalement sur une ligne donnée

   --------------------------------------------------------------------------
   Parameters:
     msg  : Message à afficher
     line : Ligne cible (1 à 8)

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void oled_display_centered(const char *msg, uint8_t line) {
    int len = strlen(msg);
    int col = (21 - len) / 2;  // Max 21 caractères par ligne
    if (col < 0) col = 0;
    ssd1306_display_text(line, msg, len, false);  // Affiche sans inversion
}


/* -------------------------------------------------------------------------- --
   FUNCTION: oled_draw_goutte

   --------------------------------------------------------------------------
   Purpose:
   Affiche une goutte avec pourcentage de remplissage simulé (debug)

   --------------------------------------------------------------------------
   Parameters:
     fill_percent : Pourcentage à afficher (0-100)

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void oled_draw_goutte(uint8_t fill_percent) {
    char buf[24];
    snprintf(buf, sizeof(buf), "[GOUTTE %d%%]", fill_percent);
    ssd1306_display_text(5, buf, strlen(buf), false);
}


/* -------------------------------------------------------------------------- --
   FUNCTION: oled_draw_explosion

   --------------------------------------------------------------------------
   Purpose:
   Affiche un message clignotant pour dépassement de temps de douche

   --------------------------------------------------------------------------
   Parameters:
     blink : Si true, affiche le message. Sinon, l'efface.

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void oled_draw_explosion(bool blink) {
    ssd1306_display_text(2, blink ? "!! DEPASSÉ !!" : "              ", 13, false);
}


/* -------------------------------------------------------------------------- --
   FUNCTION: show_boot_screen

   --------------------------------------------------------------------------
   Purpose:
   Affichage de bienvenue au démarrage de l’ESP32

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void show_boot_screen(void) {
    oled_clear();
    oled_display_centered("Bienvenue a vous !", 3);
}


/* -------------------------------------------------------------------------- --
   FUNCTION: show_bienvenue_user

   --------------------------------------------------------------------------
   Purpose:
   Affiche un message personnalisé de bienvenue à l’utilisateur

   --------------------------------------------------------------------------
   Parameters:
     username : nom reçu via BLE

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void show_bienvenue_user(const char* username) {
    oled_clear();
    char buf[40];
    snprintf(buf, sizeof(buf), "   Bonne douche a vous %s", username);
    oled_display_centered(buf, 3);
}


/* -------------------------------------------------------------------------- --
   FUNCTION: on_username_received

   --------------------------------------------------------------------------
   Purpose:
   Callback déclenché lorsqu’un nom d’utilisateur est reçu via BLE

   --------------------------------------------------------------------------
   Parameters:
     username : nom reçu en BLE

   --------------------------------------------------------------------------
   Description:
   - Stocke localement le nom
   - Met à jour l’écran OLED avec un message de bienvenue

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void on_username_received(const char* username) {
    strncpy(user_name, username, sizeof(user_name) - 1);
    user_name[sizeof(user_name) - 1] = '\0';   // Null-terminate
    show_bienvenue_user(user_name);
}
