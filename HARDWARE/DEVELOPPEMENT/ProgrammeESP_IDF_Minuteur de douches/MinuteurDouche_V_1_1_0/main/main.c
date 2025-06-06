#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "oled_display.h"
#include "ble_spp_server.h"
#include "button_handler.h"
#include "led_control.h"
#include "timer_manager.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_GPIO GPIO_NUM_5
#define TAG "MAIN"

// --- Tâche DEBUG (affiche l'état du bouton dans la console, optionnelle) ---
void test_button_task(void *arg) {
    while (1) {
        int state = gpio_get_level(BUTTON_GPIO);
        ESP_LOGI("DEBUG", "Etat bouton: %d", state);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// --- Tâche de gestion du bouton et du minuteur ---
void button_timer_task(void *arg) {
    static int last_state = 1;
    while (1) {
        int current_state = gpio_get_level(BUTTON_GPIO);
        if (last_state == 1 && current_state == 0) {
            ESP_LOGI(TAG, "Appui bouton détecté !");
            // Start/Stop minuteur sur appui
            if (timer_manager_get_state() == TIMER_STOPPED) {
                timer_manager_start("User"); // Remplace "User" par le nom réel si tu veux
            } else {
                timer_manager_stop();
            }
        }
        last_state = current_state;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void) {
    // Initialisation des périphériques
    oled_init();
    oled_display_message
    ("     Bienvenue <Nom du User>      ");
    ble_server_init();
    button_init();
    led_init();
    timer_manager_init();

    // --- Lancer la tâche de gestion bouton/minuteur ---
    xTaskCreate(button_timer_task, "button_timer_task", 2048, NULL, 5, NULL);

    // --- (Optionnel) : Lancer la tâche debug GPIO ---
    // xTaskCreate(test_button_task, "test_button", 2048, NULL, 5, NULL);

    // Boucle principale vide (tout se passe dans les tâches)
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
