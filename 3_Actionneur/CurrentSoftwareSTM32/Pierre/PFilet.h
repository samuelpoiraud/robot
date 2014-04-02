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
 * Cette fonction est appel�e en fin de match
 */
void FILET_stop();

/** G�re les messages CAN li�s au FILET.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e FILET_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void FILET_run_command(queue_id_t queueId, bool_e init);
void FILET_process_10ms(void);
void FILET_process_main(void);
void FILET_BOUTON_process(void);


#endif  /* I_AM_ROBOT_BIG */
#endif	/* PFILET_H */

