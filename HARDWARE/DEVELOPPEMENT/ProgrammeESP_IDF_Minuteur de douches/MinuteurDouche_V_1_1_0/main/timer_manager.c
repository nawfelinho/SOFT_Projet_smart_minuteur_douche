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

// --- Déclaration des variables BLE partagées (depuis ble_spp_server.c) ---
extern bool is_connected;
extern bool enable_data_ntf;
extern uint16_t spp_conn_id;
extern esp_gatt_if_t spp_gatts_if;
extern uint16_t spp_handle_table[];
#define SPP_IDX_SPP_DATA_NTY_VAL 4

// --- PARAMÈTRES ---
#define TIMER_DURATION_MS  (5 * 60 * 1000) // 5 minutes
#define OVERTIME_BLINK_MS   400            // Clignotement LED/OLED pendant dépassement

static const char* TAG = "TIMER";

static timer_state_t state = TIMER_STOPPED;
static uint32_t start_ms = 0;
static uint32_t overtime_ms = 0;
static char user_name[32] = "User";

// --- Fonctions d'accès à l'état ---
timer_state_t timer_manager_get_state(void) { return state; }
uint32_t timer_manager_get_total_time(void) {
    if (state == TIMER_STOPPED)
        return 0;
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    if ((state == TIMER_RUNNING || state == TIMER_OVERTIME) && now > start_ms)
        return now - start_ms;
    return 0;
}

// --- Démarrage du minuteur ---
void timer_manager_start(const char* username) {
    if (state == TIMER_RUNNING || state == TIMER_OVERTIME) return;
    start_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    state = TIMER_RUNNING;
    overtime_ms = 0;
    if (username) strncpy(user_name, username, sizeof(user_name)-1);
    user_name[sizeof(user_name)-1] = '\0';

    oled_display_message("Début douche !");
    led_off();
    ESP_LOGI(TAG, "Timer démarré pour %s", user_name);

    // (Optionnel) : tu peux notifier le backend ici si tu veux
}

// --- Arrêt du minuteur ---
void timer_manager_stop(void) {
    if (state == TIMER_STOPPED) return;
    uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t total_time = now - start_ms;
    state = TIMER_STOPPED;
    led_off();

    char buf[32];
    snprintf(buf, sizeof(buf), "%s %02lu:%02lu", "Total", (total_time / 60000), (total_time / 1000) % 60);
    oled_display_message(buf);

    ESP_LOGI(TAG, "Timer arrêté. Durée totale = %lu ms", (unsigned long)total_time);

    // --- Envoi BLE (à adapter si tu veux plus de détails) ---
    if (is_connected && enable_data_ntf) {
        char ble_msg[64];
        snprintf(ble_msg, sizeof(ble_msg), "User:%s;Time:%lu s", user_name, (unsigned long)(total_time / 1000));
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

// --- Tâche FreeRTOS de gestion du minuteur (affichage, clignotement, etc) ---
static void timer_manager_task(void *arg) {
    bool blink = false;
    while (1) {
        if (state == TIMER_RUNNING) {
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            uint32_t elapsed = now - start_ms;
            if (elapsed >= TIMER_DURATION_MS) {
                state = TIMER_OVERTIME;
                overtime_ms = 0;
                ESP_LOGI(TAG, "Mode dépassement ! Temps dépassé.");
                oled_display_message("Dépassement !");
                led_on();
            } else {
                uint32_t remain = (TIMER_DURATION_MS - elapsed) / 1000;
                char buf[64];
                char short_name[16];
                strncpy(short_name, user_name, sizeof(short_name)-1);
                short_name[sizeof(short_name)-1] = '\0';
                snprintf(buf, sizeof(buf), "%s %02lu:%02lu", short_name, (remain / 60), (remain % 60));
                oled_display_message(buf);
                led_off();
            }
        }
        else if (state == TIMER_OVERTIME) {
            uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            overtime_ms = now - start_ms - TIMER_DURATION_MS;
            char buf[32];
            snprintf(buf, sizeof(buf), "00:00  +%lus", (unsigned long)(overtime_ms/1000));
            oled_display_message(buf);
            // LED clignote (blink ON/OFF à chaque tick)
            if (blink)
                led_on();
            else
                led_off();
            blink = !blink;
            vTaskDelay(pdMS_TO_TICKS(OVERTIME_BLINK_MS));
            continue;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// --- À appeler au boot (dans app_main) ---
void timer_manager_init(void) {
    xTaskCreate(timer_manager_task, "timer_manager_task", 4096, NULL, 6, NULL);
}
