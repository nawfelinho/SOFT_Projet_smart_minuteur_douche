/* ========================================================================== --
                     Projet : Smart Minuteur de Douche - ESP32

   ==========================================================================
   File: user_context.h

   ==========================================================================
   Functional description:
   --------------------------------------------------------------------------
   Interface de déclaration de la variable globale `user_name`.
   Cette variable stocke le nom de l’utilisateur actif, reçu via BLE.
   Elle peut être utilisée pour l’affichage ou l’envoi de données.

   ==========================================================================
   History:
   --------------------------------------------------------------------------
   Date: 06.06.2025 - Auteur : Damien LORIGEON
   + Création pour centraliser l’accès à `user_name`
-- ========================================================================== */

#ifndef USER_CONTEXT_H
#define USER_CONTEXT_H

/**-------------------------------------------------------------------------- --
   Variable globale (déclarée dans oled_display.c)
   - Utilisée pour stocker le nom de l’utilisateur
   - Taille maximale : 31 caractères + '\0'
-- -------------------------------------------------------------------------- */
extern char user_name[32];

#endif // USER_CONTEXT_H
