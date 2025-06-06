/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: timer_manager.c

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Gestion du minuteur de douche :
   - Démarrage et arrêt du timer (5 minutes par défaut)
   - Gestion de l’affichage OLED pendant le timer et en dépassement
   - Clignotement visuel avec LED et message OLED en dépassement
   - Envoi de données via BLE à la fin de la douche
   - État global du timer accessible via getter

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Première version fonctionnelle avec clignotement et BLE

-- ========================================================================== */

/**-------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include "timer_manager.h"
#include "oled_display.h"
#include "led_control.h"
#include "ble_spp_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include "esp_gatts_api.h"

/**-------------------------------------------------------------------------- --
   External variables (depuis BLE)
-- -------------------------------------------------------------------------- */
extern bool is_connected;
extern bool enable_data_ntf;
extern uint16_t spp_conn_id;
extern esp_gatt_if_t spp_gatts_if;
extern uint16_t spp_handle_table[];
#define SPP_IDX_SPP_DATA_NTY_VAL 4

extern char user_name[32];  // Nom d’utilisateur global partagé

/**-------------------------------------------------------------------------- --
   Constants and macros
-- -------------------------------------------------------------------------- */
#define TIMER_DURATION_MS    (5 * 60 * 1000)  // Durée timer = 5 minutes
#define OVERTIME_BLINK_MS    400              // Période blink en dépassement

static const char* TAG = "TIMER";

/**-------------------------------------------------------------------------- --
   Static variables
-- -------------------------------------------------------------------------- */
static timer_state_t state = TIMER_STOPPED;   // État actuel du timer
static uint32_t start_ms = 0;                 // Timestamp du démarrage
static uint32_t overtime_ms = 0;              // Durée de dépassement

/**========================================================================== --
   Public functions
-- ========================================================================== */

/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_get_state

   --------------------------------------------------------------------------
   Purpose:
   Accès à l’état courant du timer (stopped, running, overtime)

   --------------------------------------------------------------------------
   Return value:
     timer_state_t

-- -------------------------------------------------------------------------- */
timer_state_t timer_manager_get_state(void) {
    return state;
}


/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_get_total_time

   --------------------------------------------------------------------------
   Purpose:
   Donne le temps écoulé depuis le démarrage du timer

   --------------------------------------------------------------------------
   Return value:
     Temps en millisecondes

-- -------------------------------------------------------------------------- */
uint32_t timer_manager_get_total_time(void) {
    if (state == TIMER_STOPPED)
        return 0;

    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if ((state == TIMER_RUNNING || state == TIMER_OVERTIME) && now > start_ms)
        return now - start_ms;

    return 0;
}


/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_start

   --------------------------------------------------------------------------
   Purpose:
   Démarre le timer avec un nom d’utilisateur facultatif

   --------------------------------------------------------------------------
   Parameters:
     username : chaîne reçue depuis BLE ou locale

   --------------------------------------------------------------------------
   Return value:
     Aucun

-- -------------------------------------------------------------------------- */
void timer_manager_start(const char* username) {
    if (state == TIMER_RUNNING || state == TIMER_OVERTIME) return;

    start_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    state = TIMER_RUNNING;
    overtime_ms = 0;

    if (username && username[0] != '\0') {
        strncpy(user_name, username, sizeof(user_name)-1);
        user_name[sizeof(user_name)-1] = '\0';
    }

    oled_clear();
    oled_display_centered("Debut douche !", 3);
    led_off();

    ESP_LOGI(TAG, "Timer demarre pour %s", user_name);
}


/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_stop

   --------------------------------------------------------------------------
   Purpose:
   Arrête le timer, affiche le temps total et l’envoie en BLE

   --------------------------------------------------------------------------
   Return value:
     Aucun

-- -------------------------------------------------------------------------- */
void timer_manager_stop(void) {
    if (state == TIMER_STOPPED) return;

    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t total_time = now - start_ms;

    state = TIMER_STOPPED;
    led_off();

    char buf[32];
    snprintf(buf, sizeof(buf), "%s %02u:%02u", "Total",
             (unsigned int)(total_time / 60000),
             (unsigned int)((total_time / 1000) % 60));
    oled_clear();
    oled_display_centered(buf, 3);

    ESP_LOGI(TAG, "Timer arrete. Duree totale = %lu ms", (unsigned long)total_time);

    // Envoi BLE du résumé de la douche
    if (is_connected && enable_data_ntf) {
        char ble_msg[64];
        snprintf(ble_msg, sizeof(ble_msg), "User:%s;Time:%lu s",
                 user_name, (unsigned long)(total_time / 1000));
        esp_ble_gatts_send_indicate(
            spp_gatts_if,
            spp_conn_id,
            spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL],
            strlen(ble_msg),
            (uint8_t*)ble_msg,
            false
        );
    }
}


/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_task

   --------------------------------------------------------------------------
   Purpose:
   Tâche FreeRTOS principale : actualise écran et LED en fonction de l’état

   --------------------------------------------------------------------------
   Description:
   - En mode RUNNING : affiche le temps restant et une jauge
   - En mode OVERTIME : clignote OLED et LED, compte le dépassement
   - Sinon : ne fait rien

   --------------------------------------------------------------------------
   Return value:
     Aucun (boucle infinie)

-- -------------------------------------------------------------------------- */
static void timer_manager_task(void *arg) {
    bool blink = false;
    while (1) {
        if (state == TIMER_RUNNING) {
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            uint32_t elapsed = now - start_ms;

            if (elapsed >= TIMER_DURATION_MS) {
                state = TIMER_OVERTIME;
                overtime_ms = 0;
                ESP_LOGI(TAG, "Mode depassement ! Temps depasse.");
                oled_draw_explosion(true);
                led_on();
            } else {
                uint32_t remain = (TIMER_DURATION_MS - elapsed) / 1000;
                uint8_t fill = 100 - (remain * 100) / (TIMER_DURATION_MS/1000);

                oled_clear();
                char buf[64];
                snprintf(buf, sizeof(buf), "%s %02u:%02u", user_name,
                         (unsigned int)(remain / 60), (unsigned int)(remain % 60));
                oled_display_centered(buf, 1);
                oled_draw_goutte(fill);
                led_off();
            }
        }

        else if (state == TIMER_OVERTIME) {
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            overtime_ms = now - start_ms - TIMER_DURATION_MS;

            char buf[32];
            snprintf(buf, sizeof(buf), "00:00  +%lus", (unsigned long)(overtime_ms / 1000));
            oled_draw_explosion(blink);
            oled_display_centered(buf, 5);

            if (blink) led_on();
            else       led_off();

            blink = !blink;
            vTaskDelay(pdMS_TO_TICKS(OVERTIME_BLINK_MS));
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(200));  // Actualisation régulière
    }
}


/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_init

   --------------------------------------------------------------------------
   Purpose:
   Initialise le système de minuterie (démarrage de la tâche)

   --------------------------------------------------------------------------
   Return value:
     Aucun

-- -------------------------------------------------------------------------- */
void timer_manager_init(void) {
    xTaskCreate(timer_manager_task, "timer_manager_task", 4096, NULL, 6, NULL);
}
