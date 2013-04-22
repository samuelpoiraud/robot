/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Ball_launcher.c
 *	Package : Carte actionneur
 *	Description : Asservissement du lanceur de balle
 *  Auteur : Alexis
 *  Version 20130208
 *  Robot : Krusty
 */

#ifndef BALL_LAUNCHER_H
#define	BALL_LAUNCHER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../queue.h"


/** Initialisation du gestionnaire du lanceur de balle.
 *
 * Configure DCMotor pour l'asservissement du lanceur.
 */
void BALLLAUNCHER_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void BALLLAUNCHER_stop();

/** Change la vitesse du lanceur de balle instantan�ment.
 *
 * Cette fonction est appel�e par KBall_sorter apr�s une detection de couleur.
 * Si le lanceur de balle �tait en cours d'asservissement � une vitesse donn�
 * par message CAN (et donc en passant par la pile), la nouvelle vitesse donn�e par
 * la nouvelle fonction sera prise en compte et le message CAN de resultat
 * retournera Ok quand le lanceur de balle aura atteint la nouvelle vitesse.
 */
void BALLLAUNCHER_set_speed(Uint16 tr_per_min);

/** G�re les messages CAN li�s au lanceur de balle.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'asservir le moteur.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init);

#endif	/* I_AM_ROBOT_KRUSTY */
#endif	/* BALL_LAUNCHER_H */
