/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: button_handler.c

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Gestion du bouton poussoir (GPIO)
   - Initialisation du GPIO
   - Détection d’un appui (front descendant)
   - Envoi d’une notification BLE ("BP") si connectée et notifications activées
   - Optionnel : bascule de l’état d’une LED pour retour visuel

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Première version avec gestion BLE et LED intégrée

-- ========================================================================== */

/**-------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include "button_handler.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_gatts_api.h"
#include "led_control.h"
#include <string.h>

/**-------------------------------------------------------------------------- --
   Extern variables (déclarées dans ble_spp_server.c)
-- -------------------------------------------------------------------------- */
extern bool is_connected;                      // BLE connecté ?
extern bool enable_data_ntf;                   // Notifications activées ?
extern uint16_t spp_conn_id;                   // ID de la connexion BLE
extern esp_gatt_if_t spp_gatts_if;             // Interface GATT
extern uint16_t spp_handle_table[];            // Table des handles GATT

/**-------------------------------------------------------------------------- --
   Local macros and constants
-- -------------------------------------------------------------------------- */
#define SPP_IDX_SPP_DATA_NTY_VAL 4             // Index de la caractéristique de notification (doit être identique au serveur BLE)
#define BUTTON_GPIO GPIO_NUM_5                 // Broche GPIO utilisée pour le bouton poussoir
#define TAG "BUTTON"                           // Tag pour les logs

/**========================================================================== --
   Public functions
-- ========================================================================== */

/* -------------------------------------------------------------------------- --
   FUNCTION: button_init

   --------------------------------------------------------------------------
   Purpose:
   Initialiser le GPIO du bouton poussoir

   --------------------------------------------------------------------------
   Description:
   - Configure la broche GPIO comme entrée
   - Active la résistance de pull-up interne
   - Pas d’interruption configurée ici (lecture en polling)

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void button_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),     // Masque pour la broche GPIO5
        .mode = GPIO_MODE_INPUT,                   // Configuration en entrée
        .pull_up_en = GPIO_PULLUP_ENABLE,          // Activation du pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,     // Désactivation du pull-down
        .intr_type = GPIO_INTR_DISABLE             // Pas d’interruption (polling manuel)
    };
    gpio_config(&io_conf);                         // Application de la configuration
}


/* -------------------------------------------------------------------------- --
   FUNCTION: button_check_and_log

   --------------------------------------------------------------------------
   Purpose:
   Vérifie l'état du bouton et envoie une notification BLE si appuyé

   --------------------------------------------------------------------------
   Description:
   - Fonction à appeler régulièrement (ex: dans une tâche ou boucle principale)
   - Détecte un front descendant (relâché → appuyé)
   - Si connecté au BLE et notifications activées → envoie le message "BP"
   - Active un retour visuel via la LED

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void button_check_and_log(void) {
    static int last_state = 1;                    // État précédent (1 = relâché, 0 = appuyé)
    int current_state = gpio_get_level(BUTTON_GPIO);  // Lecture de l'état actuel

    // Affiche les états BLE pour debug
    ESP_LOGI(TAG, "is_connected=%d, enable_data_ntf=%d", is_connected, enable_data_ntf);

    // Détection du front descendant (bouton appuyé)
    if (last_state == 1 && current_state == 0) {
        ESP_LOGI(TAG, "Bouton appuyé !");
        led_toggle();                             // Toggle LED pour feedback utilisateur

        // Si la connexion BLE est active et les notifications autorisées
        if (is_connected && enable_data_ntf) {
            const char *msg = "BP";               // Message à envoyer
            esp_err_t err = esp_ble_gatts_send_indicate(
                spp_gatts_if,                             // Interface BLE active
                spp_conn_id,                              // Connexion active
                spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL], // Handle de la caractéristique de notification
                strlen(msg),                              // Taille du message
                (uint8_t *)msg,                           // Données envoyées
                false                                     // false = notification (pas d'acquittement)
            );

            // Vérification de l'envoi
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Erreur d'envoi de la notification BLE : %s", esp_err_to_name(err));
            } else {
                ESP_LOGI(TAG, "Notification BLE envoyée : %s", msg);
            }
        }
    }

    last_state = current_state;  // Mise à jour de l'état pour la prochaine itération
}
