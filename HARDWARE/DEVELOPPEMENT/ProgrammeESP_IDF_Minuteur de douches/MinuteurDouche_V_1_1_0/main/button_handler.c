#include "button_handler.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_gatts_api.h"
#include "led_control.h"
#include <string.h>

// Déclaration des variables partagées avec ble_spp_server_demo.c
extern bool is_connected;
extern bool enable_data_ntf;
extern uint16_t spp_conn_id;
extern esp_gatt_if_t spp_gatts_if;
extern uint16_t spp_handle_table[];

// Index de la caractéristique de notification (doit être défini comme dans ble_spp_server_demo.c)
#define SPP_IDX_SPP_DATA_NTY_VAL 4

#define BUTTON_GPIO GPIO_NUM_5
#define TAG "BUTTON"

void button_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

void button_check_and_log(void) {
    static int last_state = 1; // État précédent du bouton
    int current_state = gpio_get_level(BUTTON_GPIO);
    
    ESP_LOGI(TAG, "is_connected=%d, enable_data_ntf=%d", is_connected, enable_data_ntf);

    if (last_state == 1 && current_state == 0) {
        ESP_LOGI(TAG, "Bouton appuyé !");
        led_toggle();

        if (is_connected && enable_data_ntf) {
            const char *msg = "BP";
            esp_err_t err = esp_ble_gatts_send_indicate(
                spp_gatts_if,
                spp_conn_id,
                spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL],
                strlen(msg),
                (uint8_t *)msg,
                false
            );

            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Erreur d'envoi de la notification BLE : %s", esp_err_to_name(err));
            } else {
                ESP_LOGI(TAG, "Notification BLE envoyée : %s", msg);
            }
        }
    }

    last_state = current_state;
}
