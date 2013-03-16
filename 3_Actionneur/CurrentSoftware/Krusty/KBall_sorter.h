/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Ball_sorter.h
 *	Package : Carte actionneur
 *	Description : Gestion de la detection des balles cerises et de leur envoi une par une.
 *  Auteur : Alexis
 *  Version 20130315
 *  Robot : Krusty
 */

#ifndef KBALL_SORTER_H
#define	KBALL_SORTER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../queue.h"

/** Initialisation du gestionnaire de l'assiette.
 *
 * Configure l'AX12 qui sert � prendre les cerises une par une
 */
void BALLSORTER_init();

/** G�re les messages CAN li�s � la gestion des cerises � envoyer.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e BALLSORTER_CAN_process_msg(CAN_msg_t* msg);

#endif  /* I_AM_ROBOT_KRUSTY */
#endif	/* KBALL_SORTER_H */
