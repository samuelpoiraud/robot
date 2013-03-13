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

#ifndef KLIFT_H
#define	KLIFT_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../queue.h"

/** Initialisation du gestionnaire des ascenseurs.
 *
 * Configure l'asservissement du moteur pour la translation d'un ascenseurs et l'AX12 qui sert la pince
 */
void LIFT_init();

/** G�re les messages CAN li�s au ascenseurs.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e LIFT_CAN_process_msg(CAN_msg_t* msg);

#endif  /* I_AM_ROBOT_KRUSTY */
#endif	/* KLIFT_H */

