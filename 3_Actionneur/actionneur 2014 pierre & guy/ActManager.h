/*  Club Robot ESEO 2012 - 2013
 *
 *	Fichier : ActManager.h
 *	Package : Carte actionneur
 *	Description : Gestion des actionneurs
 *  Auteur : Alexis, remake Arnaud
 *  Version 20130227
 */

#ifndef ACT_MANAGER_H
#define	ACT_MANAGER_H

#include "QS/QS_all.h"

//Initialise les actionneurs
void ACTMGR_init();

//Met les actionneurs en position de départ
void ACTMGR_reset_act();

// Réinitialisation des configurations des actionneurs
void ACTMGR_reset_config();

//Gère les messages CAN des actionneurs. Si le message à été géré, cette fonction renvoie TRUE, sinon FALSE.
bool_e ACTMGR_process_msg(CAN_msg_t* msg);

//Stop tous les actionneurs
void ACTMGR_stop();

//Configure un AX12 en fonction d'un message CAN
void ACTMGR_config_AX12(Uint8 id_servo, CAN_msg_t* msg);

#endif	/* ACT_MANAGER_H */
