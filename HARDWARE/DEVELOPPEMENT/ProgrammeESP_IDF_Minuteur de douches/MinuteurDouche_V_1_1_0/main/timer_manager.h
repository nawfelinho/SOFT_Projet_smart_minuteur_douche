#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <stdint.h>

typedef enum {
    TIMER_STOPPED,
    TIMER_RUNNING,
    TIMER_OVERTIME
} timer_state_t;

// À appeler pour lancer ou arrêter le minuteur (depuis bouton, etc)
void timer_manager_start(const char* username);  // username affiché
void timer_manager_stop(void);

// Initialisation + lancement de la tâche (à faire dans app_main)
void timer_manager_init(void);

// Récupérer l’état du timer
timer_state_t timer_manager_get_state(void);

// Récupérer la durée totale de la douche (en ms)
uint32_t timer_manager_get_total_time(void);

#endif
