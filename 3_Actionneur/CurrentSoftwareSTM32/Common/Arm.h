/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lift.h
 *	Package : Carte actionneur
 *	Description : Gestion des ascenseurs
 *  Auteur : Alexis
 *  Version 20130314
 *  Robot : Krusty
 */

#ifndef ARM_H
#define	ARM_H

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire des ascenseurs.
 *
 * Configure l'asservissement du moteur pour la translation d'un ascenseurs et l'AX12 qui sert la pince
 */
void ARM_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void ARM_stop();

/** G�re les messages CAN li�s au bras.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e ARM_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void ARM_run_command(queue_id_t queueId, bool_e init);

#endif	/* ARM_H */

