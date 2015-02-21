/*  Club Robot ESEO 2014 - 2015
 *	HOLLY - WOOD
 *
 *	Fichier : elevator.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'ascenseur de l'actionneur spot
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef ELEVATOR_H
#define	ELEVATOR_H

#include "../QS/QS_all.h"

#ifdef I_AM_ROBOT_BIG
#include "../queue.h"

// Initialisation de l'ELEVATOR.
void ELEVATOR_init();

// Initialisation en position de l'ELEVATOR.
void ELEVATOR_init_pos();

// Stoppe l'actionneur.
void ELEVATOR_stop();

/** Gère les messages CAN liés a la ELEVATOR.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e ELEVATOR_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void ELEVATOR_run_command(queue_id_t queueId, bool_e init);

void ELEVATOR_state_machine();

void ELEVATOR_process_it();

#endif	/* ELEVATOR_H */

#endif
