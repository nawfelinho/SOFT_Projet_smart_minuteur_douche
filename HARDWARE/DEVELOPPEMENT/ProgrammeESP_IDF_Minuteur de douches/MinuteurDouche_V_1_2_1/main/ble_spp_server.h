/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: ble_spp_server.h/.c (extrait)

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Déclarations pour le serveur BLE SPP (Serial Port Profile) :
   - Définition des index de la machine d’état d’attributs (GATT)
   - Constantes pour la gestion des buffers et formats
   - Fonction d’initialisation du serveur BLE

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Intégration SPP GATT avec index pour données, commandes, statut
-- ========================================================================== */

/**-------------------------------------------------------------------------- --
   Include header files
-- -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**-------------------------------------------------------------------------- --
   Macro definitions
-- -------------------------------------------------------------------------- */
#define spp_sprintf(s,...)           sprintf((char*)(s), ##__VA_ARGS__)  // Wrapper simplifié
#define SPP_DATA_MAX_LEN             (512)    // Taille max d’un paquet de données
#define SPP_CMD_MAX_LEN              (20)     // Taille max d’une commande reçue
#define SPP_STATUS_MAX_LEN           (20)     // Taille max d’un message de statut
#define SPP_DATA_BUFF_MAX_LEN        (2*1024) // Buffer interne global (circulaire ou tampon)

/**-------------------------------------------------------------------------- --
   Enumération de la machine d'état GATT (index des caractéristiques/valeurs)
-- -------------------------------------------------------------------------- */
enum {
    SPP_IDX_SVC,                     // Service principal SPP

    SPP_IDX_SPP_DATA_RECV_CHAR,     // Caractéristique de réception (client -> serveur)
    SPP_IDX_SPP_DATA_RECV_VAL,      // Valeur associée

    SPP_IDX_SPP_DATA_NOTIFY_CHAR,   // Caractéristique de notification (serveur -> client)
    SPP_IDX_SPP_DATA_NTY_VAL,       // Valeur à notifier
    SPP_IDX_SPP_DATA_NTF_CFG,       // Configuration des notifications (Client Characteristic Configuration Descriptor)

    SPP_IDX_SPP_COMMAND_CHAR,       // Caractéristique pour les commandes (type texte)
    SPP_IDX_SPP_COMMAND_VAL,        // Valeur des commandes

    SPP_IDX_SPP_STATUS_CHAR,        // Caractéristique de retour d’état
    SPP_IDX_SPP_STATUS_VAL,         // Valeur de statut
    SPP_IDX_SPP_STATUS_CFG,         // Configuration des notifications sur statut

#ifdef SUPPORT_HEARTBEAT
    SPP_IDX_SPP_HEARTBEAT_CHAR,     // Caractéristique de heartbeat
    SPP_IDX_SPP_HEARTBEAT_VAL,      // Valeur heartbeat
    SPP_IDX_SPP_HEARTBEAT_CFG,      // Config notification heartbeat
#endif

    SPP_IDX_NB                      // Nombre total d’éléments dans le GATT
};

/**-------------------------------------------------------------------------- --
   Public function prototypes
-- -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- --
   FUNCTION: ble_server_init

   --------------------------------------------------------------------------
   Purpose:
   Initialise le serveur BLE, configure le GATT, le service SPP et les caractéristiques

   --------------------------------------------------------------------------
   Return value:
     Aucun

-- -------------------------------------------------------------------------- */
void ble_server_init(void);
