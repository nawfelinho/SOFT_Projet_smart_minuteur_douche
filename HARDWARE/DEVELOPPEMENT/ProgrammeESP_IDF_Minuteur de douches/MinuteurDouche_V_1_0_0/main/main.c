#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "oled_display.h"
#include "ble_spp_server.h"
#include "button_handler.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_GPIO GPIO_NUM_5

// --- Tâche DEBUG (affiche l'état du bouton dans la console, optionnelle) ---
void test_button_task(void *arg) {
    while (1) {
        int state = gpio_get_level(BUTTON_GPIO);
        ESP_LOGI("DEBUG", "Etat bouton: %d", state);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// --- Tâche principale de gestion du bouton (détection + notif BLE) ---
void button_notify_task(void *arg) {
    while (1) {
        button_check_and_log();      // Appelle la logique : envoie la notif BLE si appui
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void) {
    // Initialisation de l'écran OLED
    oled_init();
    oled_display_message("Hello ESP32 !");

    // Initialisation du BLE (serveur)
    ble_server_init();

    // Initialisation du bouton (GPIO)
    button_init();

    // --- Lancer la tâche principale de gestion bouton (notification BLE) ---
    xTaskCreate(button_notify_task, "button_notify_task", 2048, NULL, 5, NULL);

    // --- (Optionnel) : Lancer la tâche debug GPIO ---
    // xTaskCreate(test_button_task, "test_button", 2048, NULL, 5, NULL);

    // Boucle principale vide (tout se passe dans les tâches)
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
