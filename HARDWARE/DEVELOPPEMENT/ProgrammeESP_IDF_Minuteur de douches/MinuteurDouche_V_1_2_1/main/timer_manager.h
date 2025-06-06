/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: timer_manager.h

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Interface pour la gestion du minuteur :
   - Lancement et arrêt de la douche
   - Gestion d’un état de dépassement de temps
   - Informations sur la durée et l’état du minuteur
   - À utiliser avec FreeRTOS

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Création de l’interface de gestion du minuteur (FreeRTOS)
-- ========================================================================== */

#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <stdint.h>

/**-------------------------------------------------------------------------- --
   Types publics
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   ENUM: timer_state_t
   Représente l’état actuel du minuteur :
     - TIMER_STOPPED  : Minuteur arrêté
     - TIMER_RUNNING  : Minuteur en cours
     - TIMER_OVERTIME : Temps dépassé
-- -------------------------------------------------------------------------- */
typedef enum {
    TIMER_STOPPED,
    TIMER_RUNNING,
    TIMER_OVERTIME
} timer_state_t;


/**-------------------------------------------------------------------------- --
   Fonctions principales
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_start
   Démarre le minuteur avec un utilisateur donné (affichage personnalisé)
   Paramètre :
     username : nom de l’utilisateur à afficher sur l’OLED
-- -------------------------------------------------------------------------- */
void timer_manager_start(const char* username);

/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_stop
   Arrête le minuteur en cours, affiche la durée et envoie la donnée via BLE
-- -------------------------------------------------------------------------- */
void timer_manager_stop(void);

/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_init
   Initialise et lance la tâche timer_manager_task (FreeRTOS)
   À appeler dans app_main
-- -------------------------------------------------------------------------- */
void timer_manager_init(void);


/**-------------------------------------------------------------------------- --
   Fonctions utilitaires
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_get_state
   Retourne l’état actuel du minuteur (voir timer_state_t)
-- -------------------------------------------------------------------------- */
timer_state_t timer_manager_get_state(void);

/* -------------------------------------------------------------------------- --
   FUNCTION: timer_manager_get_total_time
   Retourne la durée écoulée depuis le démarrage (en millisecondes)
   Si stoppé : retourne 0
-- -------------------------------------------------------------------------- */
uint32_t timer_manager_get_total_time(void);

#endif // TIMER_MANAGER_H
