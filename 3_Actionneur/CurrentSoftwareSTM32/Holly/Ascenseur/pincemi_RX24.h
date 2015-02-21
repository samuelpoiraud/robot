/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : pincemi_RX24.h
 *	Package : Carte actionneur
 *	Description : Gestion des pinces de l'actionneur spot
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef PINCEMI_RX24_H
#define	PINCEMI_RX24_H

#include "../QS/QS_all.h"

#ifdef I_AM_ROBOT_BIG
#include "../queue.h"

/** Initialisation du gestionnaire de la ELEVATOR.
 *
 * Configure l'asservissement de l'AX12
 */
void PINCEMI_init();

/** Initialisation en position du gestionnaire de la ELEVATOR.
 *
 * Initialise la position de l'AX12
 */
void PINCEMI_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void PINCEMI_stop();

/** G�re les messages CAN li�s a la ELEVATOR.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e PINCEMI_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void PINCEMI_run_command(queue_id_t queueId, bool_e init);

/**
 * R�initialise la configuration de l'AX12
 */
void PINCEMI_reset_config();

#endif	/* PINCEMI_RX24_H */

#endif

