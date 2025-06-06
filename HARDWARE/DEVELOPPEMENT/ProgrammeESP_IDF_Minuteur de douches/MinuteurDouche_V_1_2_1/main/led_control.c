/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: led_control.c

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Contrôle d'une LED connectée à une broche GPIO de l'ESP32
   - Initialisation du GPIO en sortie
   - Fonctions pour allumer, éteindre, basculer et lire l’état de la LED

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Première version avec état interne et API simple

-- ========================================================================== */

/**-------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include "led_control.h"
#include "driver/gpio.h"

/**-------------------------------------------------------------------------- --
   Static variables
-- -------------------------------------------------------------------------- */
static int led_state = 0;   // État interne de la LED : 0 = éteinte, 1 = allumée

/**========================================================================== --
   Public functions
-- ========================================================================== */

/* -------------------------------------------------------------------------- --
   FUNCTION: led_init

   --------------------------------------------------------------------------
   Purpose:
   Initialiser le GPIO utilisé pour la LED

   --------------------------------------------------------------------------
   Description:
   - Configure le GPIO comme sortie
   - Désactive les pull-up/pull-down
   - Éteint la LED par défaut

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void led_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),       // Sélection de la broche
        .mode = GPIO_MODE_OUTPUT,                 // Mode sortie
        .pull_up_en = GPIO_PULLUP_DISABLE,        // Pas de pull-up
        .pull_down_en = GPIO_PULLDOWN_DISABLE,    // Pas de pull-down
        .intr_type = GPIO_INTR_DISABLE            // Pas d’interruption
    };
    gpio_config(&io_conf);   // Applique la configuration GPIO
    led_off();               // État par défaut : LED éteinte
}


/* -------------------------------------------------------------------------- --
   FUNCTION: led_on

   --------------------------------------------------------------------------
   Purpose:
   Allumer la LED

   --------------------------------------------------------------------------
   Description:
   - Met la sortie GPIO au niveau haut
   - Met à jour l’état interne

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void led_on(void) {
    gpio_set_level(LED_GPIO, 1);   // GPIO à l’état haut
    led_state = 1;                 // État interne = allumé
}


/* -------------------------------------------------------------------------- --
   FUNCTION: led_off

   --------------------------------------------------------------------------
   Purpose:
   Éteindre la LED

   --------------------------------------------------------------------------
   Description:
   - Met la sortie GPIO au niveau bas
   - Met à jour l’état interne

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void led_off(void) {
    gpio_set_level(LED_GPIO, 0);   // GPIO à l’état bas
    led_state = 0;                 // État interne = éteint
}


/* -------------------------------------------------------------------------- --
   FUNCTION: led_toggle

   --------------------------------------------------------------------------
   Purpose:
   Inverser l’état de la LED

   --------------------------------------------------------------------------
   Description:
   - Si la LED est allumée, l’éteint
   - Si la LED est éteinte, l’allume

   --------------------------------------------------------------------------
   Return value:
   Aucun

-- -------------------------------------------------------------------------- */
void led_toggle(void) {
    if (led_state) {
        led_off();    // Si allumée → éteint
    } else {
        led_on();     // Si éteinte → allume
    }
}


/* -------------------------------------------------------------------------- --
   FUNCTION: led_get_state

   --------------------------------------------------------------------------
   Purpose:
   Lire l’état actuel de la LED

   --------------------------------------------------------------------------
   Description:
   - Renvoie la valeur de la variable interne `led_state`

   --------------------------------------------------------------------------
   Return value:
     0 : LED éteinte
     1 : LED allumée

-- -------------------------------------------------------------------------- */
int led_get_state(void) {
    return led_state;
}
