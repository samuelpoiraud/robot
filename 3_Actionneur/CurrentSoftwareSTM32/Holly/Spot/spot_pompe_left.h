/*  Club Robot ESEO 2013 - 2014
 *	BIG
 *
 *	Fichier : spot_pompe_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef PSPOT_POMPE_LEFT_H
#define	PSPOT_POMPE_LEFT_H

#include "../QS/QS_all.h"

#ifdef I_AM_ROBOT_BIG

#include "../queue.h"

/** Initialisation du gestionnaire de SPOT_POMPE_LEFT.
 *
 * Configure du moteur de la SPOT_POMPE_LEFT
 */
void SPOT_POMPE_LEFT_init();

/** Initialisation en position du gestionnaire de SPOT_POMPE_LEFT.
 *
 * Initialise de la position de la SPOT_POMPE_LEFT (stopé)
 */
void SPOT_POMPE_LEFT_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appelée en fin de match
 */
void SPOT_POMPE_LEFT_stop();

/** Gère les messages CAN liés au SPOT_POMPE_LEFT.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e SPOT_POMPE_LEFT_CAN_process_msg(CAN_msg_t* msg);

/**
 * Réinitialise la configuration de l'AX12
 */
void SPOT_POMPE_LEFT_reset_config();

#endif

#endif	/* PSPOT_POMPE_LEFT_H */

