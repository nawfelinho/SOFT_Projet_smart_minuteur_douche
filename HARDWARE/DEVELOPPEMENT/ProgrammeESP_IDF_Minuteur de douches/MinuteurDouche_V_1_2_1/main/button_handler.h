/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: button_handler.h

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Interface du module de gestion du bouton poussoir :
   - Initialisation du GPIO du bouton
   - Détection des appuis et déclenchement d'actions (logique + BLE)

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Création initiale du header pour la gestion du bouton
-- ========================================================================== */

#pragma once

/**-------------------------------------------------------------------------- --
   Public function prototypes
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: button_init

   --------------------------------------------------------------------------
   Purpose:
   Initialise le GPIO associé au bouton en mode entrée avec pull-up activé

   --------------------------------------------------------------------------
   Return value:
     Aucun

-- -------------------------------------------------------------------------- */
void button_init(void);

/* -------------------------------------------------------------------------- --
   FUNCTION: button_check_and_log

   --------------------------------------------------------------------------
   Purpose:
   Vérifie l'état du bouton et déclenche une action à l'appui :
   - Toggle de la LED
   - Envoi d'une notification BLE si connectée

   --------------------------------------------------------------------------
   Description:
   À appeler régulièrement dans une boucle ou une tâche FreeRTOS

   --------------------------------------------------------------------------
   Return value:
     Aucun

-- -------------------------------------------------------------------------- */
void button_check_and_log(void);
