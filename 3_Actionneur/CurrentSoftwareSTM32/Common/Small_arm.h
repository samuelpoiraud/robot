/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : PSmallArm.h
 *	Package : Carte actionneur
 *	Description : Gestion du petit bras
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PSMALL_ARM_H
#define	PSMALL_ARM_H

#include "../QS/QS_all.h"

#include "../queue.h"

typedef enum {
	SMALL_ARM_CS_IdleAX12,
	SMALL_ARM_CS_MidAX12,
	SMALL_ARM_CS_DeployedAX12,
	SMALL_ARM_CS_StopAX12
} SMALL_ARM_command_state_e;

/** Initialisation du gestionnaire de SMALL_ARM.
 *
 * Configure l'asservissement de l'AX12
 */
void SMALL_ARM_init();

/** Initialisation en position du gestionnaire de SMALL_ARM.
 *
 * Initialise la position de l'AX12
 */
void SMALL_ARM_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void SMALL_ARM_stop();

/** G�re les messages CAN li�s au SMALL_ARM.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e SMALL_ARM_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void SMALL_ARM_run_command(queue_id_t queueId, bool_e init);

#endif	/* PSMALL_ARM_H */

