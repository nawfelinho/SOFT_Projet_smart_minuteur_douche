/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: main.c

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Fichier principal de l'application ESP32.
   Initialise les modules (OLED, BLE, bouton, LED, minuteur) et
   lance les tâches FreeRTOS nécessaires au fonctionnement du système.

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Version initiale avec affichage, minuteur et BLE

-- ========================================================================== */

/**-------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "oled_display.h"
#include "ble_spp_server.h"
#include "button_handler.h"
#include "led_control.h"
#include "timer_manager.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "ssd1306.h"

/**-------------------------------------------------------------------------- --
   Local constants and macros
-- -------------------------------------------------------------------------- */
#define BUTTON_GPIO GPIO_NUM_5   // GPIO utilisé pour le bouton poussoir
#define TAG "MAIN"               // Tag utilisé pour les logs

/**-------------------------------------------------------------------------- --
   External variables
-- -------------------------------------------------------------------------- */
extern char user_name[32];       // Nom utilisateur reçu via BLE

/**-------------------------------------------------------------------------- --
   Private functions
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: test_button_task

   --------------------------------------------------------------------------
   Purpose:
   Tâche de DEBUG affichant l’état du bouton GPIO dans les logs

   --------------------------------------------------------------------------
   Description:
   Affiche l'état (0 ou 1) du bouton toutes les 200 ms. Utilisable pour vérifier
   le câblage et le rebond du bouton.

   --------------------------------------------------------------------------
   Return value:
   Aucune (fonction FreeRTOS infinie)

-- -------------------------------------------------------------------------- */
void test_button_task(void *arg) {
    while (1) {
        int state = gpio_get_level(BUTTON_GPIO);
        ESP_LOGI("DEBUG", "Etat bouton: %d", state);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/* -------------------------------------------------------------------------- --
   FUNCTION: button_timer_task

   --------------------------------------------------------------------------
   Purpose:
   Tâche principale de gestion des appuis bouton et du minuteur

   --------------------------------------------------------------------------
   Description:
   - Sur appui bouton (front descendant), démarre ou arrête le minuteur
   - Vérifie que le nom utilisateur est défini avant le démarrage
   - Affiche un message d’erreur si aucun utilisateur sélectionné

   --------------------------------------------------------------------------
   Return value:
   Aucune (fonction FreeRTOS infinie)

-- -------------------------------------------------------------------------- */
void button_timer_task(void *arg) {
    static int last_state = 1; // 1 = relâché, 0 = appuyé précédemment
    while (1) {
        int current_state = gpio_get_level(BUTTON_GPIO);

        // Détection de l'appui (front descendant)
        if (last_state == 1 && current_state == 0) {
            ESP_LOGI(TAG, "Appui bouton détecté !");

            if (timer_manager_get_state() == TIMER_STOPPED) {
                // Vérifie que l'utilisateur est bien sélectionné
                if (user_name[0] == '\0' || strcmp(user_name, "User") == 0) {
                    ssd1306_clear_screen();
                    ssd1306_display_text(2, "Selectionnez un", strlen("Selectionnez un"), false);
                    ssd1306_display_text(3, "utilisateur avant", strlen("utilisateur avant"), false);
                    ssd1306_display_text(4, "la douche", strlen("la douche"), false);
                } else {
                    // Lancement du minuteur avec le nom utilisateur
                    timer_manager_start(user_name);
                }
            } else {
                // Arrêt du minuteur
                timer_manager_stop();
            }
        }

        last_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* -------------------------------------------------------------------------- --
   FUNCTION: app_main

   --------------------------------------------------------------------------
   Purpose:
   Point d'entrée principal de l'application ESP32

   --------------------------------------------------------------------------
   Description:
   - Initialise tous les composants logiciels et matériels
   - Lance les tâches FreeRTOS (bouton + timer)
   - Affiche l’écran d’accueil à l'initialisation

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void app_main(void) {
    // Initialisation des périphériques
    oled_init();
    show_boot_screen();   // Affiche l'écran de bienvenue

    ble_server_init();    // Initialise le serveur BLE
    button_init();        // Configure le GPIO bouton
    led_init();           // Prépare la LED de signalisation
    timer_manager_init(); // Lance la tâche de gestion du timer

    // Tâche de gestion des appuis et du minuteur
    xTaskCreate(button_timer_task, "button_timer_task", 2048, NULL, 5, NULL);

    // (Optionnel) : activer pour tester l’état du bouton
    // xTaskCreate(test_button_task, "test_button", 2048, NULL, 5, NULL);

    // Boucle infinie principale (inactive ici)
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
