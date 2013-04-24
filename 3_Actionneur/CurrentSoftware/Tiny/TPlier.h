/*  Club Robot ESEO 2012 - 2013
 *	Tiny
 *
 *	Fichier : Plier.h
 *	Package : Carte actionneur
 *	Description : Gestion des bras horizontaux de tiny pour prendre des verres
 *  Auteur : Alexis
 *  Version 20130312
 *  Robot : Tiny
 */

#ifndef TPLIER_H
#define	TPLIER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_TINY

#include "../queue.h"

//Etape de l'ouverture et fermeture des pinces
//A mettre dans le param dans la queue
typedef enum {
	PLIER_CS_OpenLeftAX12,
	PLIER_CS_OpenRightAX12,
	PLIER_CS_CloseLeftAX12,
	PLIER_CS_CloseRightAX12
} PLIER_command_state_e;

/** Initialisation du gestionnaire des pinces à verres.
 *
 * Configure les AX12 de la pince.
 */
void PLIER_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void PLIER_stop();

/** Récupère la position courant du bras en degrés.
 */
Uint16 PLIER_get_pos();

/** Gère les messages CAN liés au bras.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et de bouger les pinces dans l'ordre.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e PLIER_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void PLIER_run_command(queue_id_t queueId, bool_e init);

#endif  /* I_AM_ROBOT_TINY */

#endif	/* TPLIER_H */

