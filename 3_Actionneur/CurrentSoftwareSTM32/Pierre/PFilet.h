/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : filet.h
 *	Package : Carte actionneur
 *	Description : Gestion du filet
 *  Auteur : Elise FERCHAUD
 *  Version 20130219
 *  Robot : BIG
 */

#ifndef PFILET_H
#define	PFILET_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_BIG

#include "../queue.h"

/** Initialisation du gestionnaire de FILET.
 *
 * Configure l'asservissement de l'AX12
 */
void FILET_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void FILET_stop();

/** Gère les messages CAN liés au FILET.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e FILET_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void FILET_run_command(queue_id_t queueId, bool_e init);
void FILET_process_10ms(void);
void FILET_process_main(void);
void FILET_BOUTON_process(void);


#endif  /* I_AM_ROBOT_BIG */
#endif	/* PFILET_H */

