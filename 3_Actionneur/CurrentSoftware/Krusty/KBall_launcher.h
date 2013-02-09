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

/** Gère les messages CAN liés au lanceur de balle.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'asservir le moteur.
 * (TODO: Ajouter la gestion de passage des balles)
 * @param msg le message CAN
 */
void BALLLAUNCHER_CAN_process_msg(CAN_msg_t* msg);

/**
 * Active ou non le moteur du lanceur de balle.
 * Cette fonction est appelée par QUEUE_run(), elle ne devrait pas être appelée à la main.
 * Quand init est TRUE, la commande est envoyé au module DCMotor
 * @param queueId le numéro de la queue
 * @param init TRUE si la commande doit être initialisée, FALSE sinon
 */
void BALLLAUNCHER_run_command(queue_id_t queueId, bool_e init);

#endif	/* I_AM_ROBOT_KRUSTY */
#endif	/* BALL_LAUNCHER_H */
