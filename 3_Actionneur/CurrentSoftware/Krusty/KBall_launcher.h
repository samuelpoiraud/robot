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
 * Cette fonction est appelée en fin de match
 */
void BALLLAUNCHER_stop();

/** Change la vitesse du lanceur de balle instantanément.
 *
 * Cette fonction est appelée par KBall_sorter après une detection de couleur.
 * Si le lanceur de balle était en cours d'asservissement à une vitesse donné
 * par message CAN (et donc en passant par la pile), la nouvelle vitesse donnée par
 * la nouvelle fonction sera prise en compte et le message CAN de resultat
 * retournera Ok quand le lanceur de balle aura atteint la nouvelle vitesse.
 */
void BALLLAUNCHER_set_speed(Uint16 tr_per_min);

/** Gère les messages CAN liés au lanceur de balle.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'asservir le moteur.
 * @param msg le message CAN
 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
 */
bool_e BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg);

/** Gère les commandes demandées.
 *
 * Fonction à mettre sur la file pour demander une action.
 */
void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init);

#endif	/* I_AM_ROBOT_KRUSTY */
#endif	/* BALL_LAUNCHER_H */
